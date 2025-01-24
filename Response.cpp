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
    _textBody = body;
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

