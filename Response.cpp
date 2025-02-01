#include "Response.hpp"

void Response::setHttpVersion(const std::string &version)
{
    _httpVersion = version;
}

void Response::setStatusCode(int status)
{
    _statusCode = status;
}

void Response::setReasonPhrase(const std::string &phrase)
{
    _reasonPhrase = phrase;
}

void Response::addHeader(const std::string &key, std::string &value)
{
    _headers[key] = value;
}

void Response::setTextBody(const std::string &body)
{
    _file = "";
    _textBody = body;
}

void Response::setFile(const std::string &filepath)
{
    _textBody = "";
    std::ifstream f(filepath.c_str());
    if (f.is_open())
    {
        /*std::cout << "sending file..." << std::endl;*/
        std::stringstream fileStream;
        fileStream << f.rdbuf();
        _file = fileStream.str();
    }
    else
    {
        _file = "";
        std::cout << "ERROROORORO\n";
    }
    // else throw exception (or declare file not found somehow)
}

void Response::setContentLength()
{
    std::stringstream ss;
    if (!_file.empty())
        ss << _file.size();
    else
        ss << _textBody.size();
    std::string len = ss.str();
    addHeader(std::string("Content-Length"), len);
}

<<<<<<< Updated upstream
#define RESPONSE_CHUNCK_SIZE 100
=======
int Response::buildResponse(Request &request)
{
    // Response response;
    std::string path("./assets");
    
    if (request.getRequestTarget() == "/")
        path += "/index.html";
    else
        path += request.getRequestTarget();
    std::ifstream file(path.c_str());

    std::cout << "PATH: "  << path << std::endl;

    if (request.getRequestTarget() == "/redirection")
    {
	std::string location = "https://www.youtube.com/watch?v=vmDIwhP6Q18&list=RDQn-UcLOWOdM&index=2";

        std::cout << "Ridddddaryrikchn" << std::endl;
        path = "./assets/302.html";
        std::string connection = "close";
        std::string contentType = getMimeType(path);

        setHttpVersion(HTTP_VERSION);
        setStatusCode(302);
        setReasonPhrase("Moved Temporarily");
        setFile(path);

        addHeader(std::string("Connection"), connection);
        /*response.sendResponse(new_fd);*/
        // int sent = 0;
        // while (!sent)
        //     sent = sendResponse(_sock);
    }
    else if (!file.good()) {
        std::cout << "iror nat found" << std::endl;
        path = "./assets/404.html";
        std::string connection = "close";
        std::string contentType = getMimeType(path);

        setHttpVersion(HTTP_VERSION);
        setStatusCode(404);
        setReasonPhrase("Not Found");
        setFile(path);

        setContentLength();
        addHeader(std::string("Content-Type"), contentType);
        addHeader(std::string("Connection"), connection);
        /*response.sendResponse(new_fd);*/
        // int sent = 0;
        // while (!sent)
        //     sent = sendResponse(_sock);
    }
    else
    {
        std::cout << "saad t3asb "  << path << std::endl;

        std::string connection = "close";
        std::string contentType = getMimeType(path);

        setHttpVersion(HTTP_VERSION);
        setStatusCode(200);
        setReasonPhrase("OK");
        setFile(path);

        setContentLength();
        addHeader(std::string("Content-Type"), contentType);
        addHeader(std::string("Connection"), connection);
        /*response.sendResponse(new_fd);*/
        // int sent = 0;
        // while (!sent)
        //     sent = sendResponse(_sock);
    }
    _progress = SEND_RESPONSE;
    return (0);
}

#define RESPONSE_CHUNCK_SIZE 1024
>>>>>>> Stashed changes

int Response::sendResponse(int clientSocket)
{
    if (_sent)
        return (1);

    if (_response.empty())
    {
        // Build the response line
        std::ostringstream responseStream;
        responseStream << _httpVersion << " " << _statusCode << " " << _reasonPhrase << "\r\n";

        // Add headers
        std::map<std::string, std::string>::const_iterator it;
        for (it = _headers.begin(); it != _headers.end(); it++)
        {
            responseStream << it->first << ": " << it->second << "\r\n";
        }
        responseStream << "\r\n";
        if (!_file.empty())
            responseStream << _file;
        else if (!_textBody.empty())
            responseStream << _textBody;

        _response = responseStream.str();
    }

    // Send the response in chunks
    /*ssize_t bytesSent = send(clientSocket, _response.c_str() + _totalBytesSent, _response.size() - _totalBytesSent, 0);*/
    size_t remaining = _response.size() - _totalBytesSent;
    ssize_t bytesSent = send(clientSocket, _response.c_str() + _totalBytesSent, (RESPONSE_CHUNCK_SIZE < remaining) ? RESPONSE_CHUNCK_SIZE : remaining, 0);
    if (bytesSent == -1)
    {
        perror("Failed to send response");
        return (-1);
    }
    _totalBytesSent += bytesSent;
    std::cout << "***********Sent: " << _totalBytesSent << std::endl;
    if (_totalBytesSent == _response.size())
        _sent = true, _progress = FINISHED;
    return (_sent);
}

