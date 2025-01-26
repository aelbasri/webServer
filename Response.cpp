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

void Response::sendResponse(int clientSocket) const
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

    std::string response = responseStream.str();

    // Send the response in chunks
    size_t totalBytesSent = 0;
    while (totalBytesSent < response.size())
    {
        ssize_t bytesSent = send(clientSocket, response.c_str() + totalBytesSent, response.size() - totalBytesSent, 0);
        if (bytesSent == -1)
        {
            perror("Failed to send response");
            return;
        }
        totalBytesSent += bytesSent;
    }
}

