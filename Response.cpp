#include "Response.hpp"
#include "configfile/location.hpp"
#include <clocale>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


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

int Response::buildResponse2(Request &request, server *serv)
{
    if (!serv)
    {
        std::cout << "Server not found" << std::endl;
        // return 500
        setError(500, "Internal Server Error", *this, serv);
        return (1);
    }
    // find the location match
    location* locationMatch = getLocationMatch(
                                    request.getRequestTarget(),
                                    serv->GetLocations(),
                                    serv->Get_number_of_location());
    if (locationMatch == nullptr)
    {
        std::cout << "Location not found" << std::endl;
        // return 404
        setError(404, "Not Found", *this, serv);
        return (1);
    }
    std::cout << "Matched location: " << locationMatch->GetType_of_location() << std::endl;

    // check if the file exists
    std::string path = locationMatch->GetRoot_directory();
    if ((path.size() > 0 && request.getRequestTarget().size() > 0) && path[path.size() - 1] != '/' && request.getRequestTarget()[0] != '/')
        path += "/";
    path += request.getRequestTarget();
    std::ifstream file(path.c_str());
    std::cout << "Looking for file: " << path << std::endl;
    if (!file.good())
    {
        std::cout << "File not found" << std::endl;
        // return 404
        setError(404, "Not Found", *this, serv);
        return (1);
    }
    std::cout << "File found " << path << std::endl;
    file.close();

    // check if the method is allowed
    if (methodAllowed(request.getMethod(), locationMatch->GetAllowed_methods()) == false)
    {
        std::cout << "Method not allowed" << std::endl;
        // return 405
        setError(405, "Method Not Allowed", *this, serv);
        return (1);
    }
    std::cout << "Method allowed: " << request.getMethod() << std::endl;


    // check if the file is a directory
    struct stat fileStat;
    if (stat(path.c_str(), &fileStat) < 0)
    {
        std::cout << "Stat error" << std::endl;
        // return 500
        setError(500, "Internal Server Error", *this, serv);
        return (1);
    }
    if (S_ISDIR(fileStat.st_mode))
    {
        std::cout << "Is a directory" << std::endl;
        // if autoindex is on, return the list of files, else
        // handle index files
        path += locationMatch->GetIndex();
    }
    else if (S_ISREG(fileStat.st_mode))
    {
        std::cout << "Is a file" << std::endl;
    }
    else
    {
        std::cout << "Not a file or directory" << std::endl;
        // return 500
        setError(500, "Internal Server Error", *this, serv);
        return (1);
    }

    // check if the file exists
    std::cout << "Now looking for file: " << path << std::endl;
    std::ifstream file2(path.c_str());
    if (!file2.good())
    {
        std::cout << "File not found" << std::endl;
        // return 404
        setError(404, "Not Found", *this, serv);
        return (1);
    }

    std::cout << "File found " << path << std::endl;
    // generate response
    std::string connection = "close";
    std::string contentType = getMimeType(path);

    setHttpVersion(HTTP_VERSION);
    setStatusCode(200);
    setReasonPhrase("OK");
    setFile(path);

    setContentLength();
    addHeader(std::string("Content-Type"), contentType);
    addHeader(std::string("Connection"), connection);

    return (0);
}

int Response::createResponseStream()
{
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

    _progress = SEND_RESPONSE;
    return (0);
}

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

        addHeader(std::string("Location"), location);
        addHeader(std::string("Connection"), connection);
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
    }

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

    _progress = SEND_RESPONSE;
    return (0);
}
