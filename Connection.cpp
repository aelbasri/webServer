#include "Connection.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "colors.hpp"

void Connection::sockRead()
{
    long bytesRec = 0;
    // std::cout << "BODY:" << "offset: " << _request.getOffset() << ", bytesRec:" <<  _request.getBytesRec() << std::endl;
    if (_request.getState() ==  WAIT)
        return ;
    if((_request.getOffset() >= _request.getBytesRec()))
    {
        _request.setBuffer();
        if ((bytesRec = recv(_socket, _request.getBuffer(), BUFF_SIZE, 0)) <= 0)
        {
            _request.setOffset(0);
            _request.setBytrec(0);
            _request.setBuffer();
            return ;
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
        _response.createResponseStream();
    }

    //if the request is done or waiting
    if (_request.getState() == DONE || _request.getState() == WAIT)
        _readyToWrite = true;

    //after completing the request
    if(_request.getState() == DONE)
        _request.closeContentFile();
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
        }
        if (_response.getProgress() != POST_HOLD)
            _response.createResponseStream();
    }
    if (_response.getProgress() == SEND_RESPONSE)
    {
        ssize_t bytesSent = send(_socket, _response.getResponse().c_str() + _response.getTotalBytesSent(), _response.getResponse().size() - _response.getTotalBytesSent(), 0);
        if (bytesSent == -1)
            return (-1);
        _response.setTotalBytesSent(_response.getTotalBytesSent() + bytesSent);
        if (_response.getTotalBytesSent() == _response.getResponse().size())
        {
            _response.setSent(true);
            _response.setProgress(FINISHED);
            // std::cout << "Response Done" << std::endl;
        }
    }
    return (0);
}
