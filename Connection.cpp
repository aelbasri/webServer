#include "Connection.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "colors.hpp"
#include "log.hpp"

std::string getSizeInHex(size_t size)
{
    std::stringstream stream;
    stream << std::hex << size;
    return stream.str();
}
ssize_t sendChunk(const char *buffer, size_t size, int socket, bool sendInChunkFormat)
{
    if (sendInChunkFormat)
    {
        std::stringstream hexStream;
        hexStream << std::hex << size;
        std::string sizeInHex = hexStream.str();

        std::string chunk = sizeInHex + "\r\n";
        chunk.append(buffer, size);
        chunk.append("\r\n");
        // std::cout << "Sending chunk: size=" << sizeInHex 
        //           << ", actual size=" << size 
        //           << ", total chunk size=" << chunk.size() << std::endl;
        return send(socket, chunk.c_str(), chunk.size(), MSG_NOSIGNAL);
    }
    return send(socket, buffer, size, MSG_NOSIGNAL);
}

bool Connection::sendRawBody()
{
    if (_response.getTextBody().empty()) {
        // Nothing to send
        std::cout << "AYOUB KHAWI" << std::endl;
        return true;
    }

    // Send the content
    ssize_t bytesSent = send(_socket, _response.getTextBody().data(), _response.getTextBody().size(), MSG_NOSIGNAL);
    if (bytesSent < 0) {
        // Error occurred while sending
        // std::cout << "AYOUB INTERNAL IROR" << std::endl;
        throw server::InternalServerError();
    }

    // Update the httpBodyContent to remove the sent part
    _response.setTextBody(_response.getTextBody().erase(0, bytesSent));
    // Return true if all data was sent, false otherwise
    std::cout  << bytesSent <<  _response.getTextBody() << std::endl;
    return _response.getTextBody().empty();
}

int Connection::sendFile(bool sendInChunkFormat)
{
    // std::cout << "sendi aw9" << std::endl;
    responseBodyFile *rfs = _response.getFileBody();
    if (!rfs)
        throw server::InternalServerError();
    
    // check wach baqi chi 7aja
    if (rfs->consumed < rfs->nBytes)
    {
        ssize_t nBytesSent = sendChunk(rfs->buffer + rfs->offset, rfs->nBytes - rfs->offset, _socket, sendInChunkFormat);
        if (nBytesSent == -1)
            throw server::InternalServerError();
        rfs->consumed += nBytesSent;
        rfs->offset += nBytesSent;
        return (nBytesSent);
    }

    if (rfs->file.eof())
    {
        // send last chunk
        if (sendInChunkFormat)
        {
            ssize_t nBytesSent = sendChunk("", 0, _socket, sendInChunkFormat);
            if (nBytesSent == -1)
                throw server::InternalServerError();
        }

        return (0);
    }
    rfs->file.read(rfs->buffer, BUFFER_SIZE);
    int bytesRead = rfs->file.gcount();
    // std::cout << "9RINA " << bytesRead << std::endl;
    if (bytesRead < 0)
        throw server::InternalServerError();
    rfs->nBytes = bytesRead;
    if (bytesRead > 0)
    {
        ssize_t bytesSent = sendChunk(rfs->buffer, bytesRead, _socket, sendInChunkFormat);
        if (bytesSent == -1)
        {
            std::cout << "iror azbi" << std::endl;
            throw server::InternalServerError();
        }
        rfs->consumed += bytesSent;
        rfs->offset += bytesSent;
        return (bytesSent);
    }
    return (0);
}

int Connection::sockRead()
{
    long bytesRec = 0;
    if (_request.getState() ==  WAIT)
        return (0);
    if((_request.getOffset() >= _request.getBytesRec()))
    {
        _request.setBuffer();
        if ((bytesRec = recv(_socket, _request.getBuffer(), BUFF_SIZE, 0)) <= 0)
        {
            if (bytesRec == 0)
            {
                std::cout << "EOFFOFOFO" << std::endl;
            }
            else
                std::cout << "irororor" << std::endl;
            // throw bad request if error
            _request.setOffset(0);
            _request.setBytrec(0);
            _request.setBuffer();
            return (-1);
        }
        // write(1, _request.getBuffer(), BUFF_SIZE);
    }
    try
    {
        if (_request.getOffset() >= _request.getBytesRec())
        {
            _request.setOffset(0);
            _request.setBytrec(bytesRec);
        }

        _request.handle_request(_request.getBuffer());
    }
    catch (const Request::badRequest &e)
    {
        // std::cerr << e.what() << std::endl;
        _request.setState(DONE);
        setHttpResponse(400, "Bad Request", _response, _server);
        std::string conn = "close";
        _response.createResponseStream(conn);
        std::string logMessage = "[" + _request.getMethod() + "] [" + _request.getRequestTarget() + "] [400] [Bad Request] [Request is invalid]";
        webServLog(logMessage, WARNING);
    }
    catch (const server::InternalServerError &e)
    {
        _request.setState(DONE);
        setHttpResponse(500, "Internal Server Error", _response, _server);
        std::string conn = "close";
        _response.createResponseStream(conn);
        std::string logMessage = "[" + _request.getMethod() + "] [" + _request.getRequestTarget() + "] [500] [Internal Server Error] [Open content file failed]";
        webServLog(logMessage, ERROR);
    }

    //if the request is done or waiting
    if (_request.getState() == DONE || _request.getState() == WAIT)
        _readyToWrite = true;

    //after completing the request
    if(_request.getState() == DONE)
        _request.closeContentFile();

    return (0);
}

int Connection::sockWrite()
{
    if ((_request.getState() != DONE && _request.getState() != WAIT) || _response.getProgress() == FINISHED)
        return (0);
    if (_response.getProgress() == BUILD_RESPONSE || _response.getProgress() == POST_HOLD)
    {
        try {
            _response.buildResponse(_request, _server);
        } catch (const server::InternalServerError &e) {
            setHttpResponse(500, "Internal Server Error", _response, _server);
        } catch(const Request::badRequest &e) {
            setHttpResponse(400, "Bad Request", _response, _server);
        }
        if (_response.getProgress() != POST_HOLD)
        {
            std::string conn = _request.getHeader("Connection");
            _response.createResponseStream(conn);
        }
    }
    if (_response.getProgress() == SEND_HEADERS)
    {
        ssize_t bytesSent = send(_socket, _response.getResponse().c_str() + _response.getTotalBytesSent(), _response.getResponse().size() - _response.getTotalBytesSent(), MSG_NOSIGNAL);
        if (bytesSent == -1)
            return (-1);
        _response.setTotalBytesSent(_response.getTotalBytesSent() + bytesSent);
        if (_response.getTotalBytesSent() == _response.getResponse().size())
            _response.setProgress(SEND_BODY);
    }
    else if (_response.getProgress() == SEND_BODY)
    {
        try {
            if (_response.getStatusCode() == 204)
            {
                _response.setSent(true);
                _response.setProgress(FINISHED);
                std::string logMessage = "[" + _request.getMethod() + "] [" + _request.getRequestTarget() + "] [NO BODY CONTENT SENT]";
                webServLog(logMessage, INFO);
                return (0);
            }
            else if (_response.getIsFile())
            {
                std::cout << "TABON MO HOWA" << std::endl;

                if (_response.getFileBody() == nullptr || sendFile(_response.getFileBody()->fileSize > 1024) != 0)
                    return (0);
                _response.setSent(true);
                _response.setProgress(FINISHED);
                std::string logMessage = "[" + _request.getMethod() + "] [" + _request.getRequestTarget() + "] [RESPONSE BODY FILE SENT SUCCESSFULLY]";
                webServLog(logMessage, INFO);
                return (0);
            }
            else if (sendRawBody() == true)
            {
                 _response.setSent(true);
                _response.setProgress(FINISHED);
                std::string logMessage = "[" + _request.getMethod() + "] [" + _request.getRequestTarget() + "] [RESPONSE BODY CUSTOMHTML SENT SUCCESSFULLY]";
                webServLog(logMessage, INFO);
                return (0);

            }
        } catch (const server::InternalServerError &e) {
            //setHttpResponse(500, "Internal Server Error", _response, _server);
            std::string logMessage = "[" + _request.getMethod() + "] [" + _request.getRequestTarget() + "] [500] [Internal Server Error] [Error sending request body]";
            webServLog(logMessage, ERROR);
            return (-1);
        }
    }
    return (0);
}
