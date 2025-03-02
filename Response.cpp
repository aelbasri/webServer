#include "Response.hpp"
#include "configfile/location.hpp"
#include <clocale>
#include <cmath>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "log.hpp"


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

void Response::addHeader(const std::string &key, const std::string &value)
{
    _headers[key] = value;
}

void Response::setTextBody(const std::string &body)
{
    _file = "";
    _textBody = body;
}

int Response::setFileBody(std::string path) {
    _fileBody = new responseBodyFile();
    _fileBody->file.open(path.c_str());
    if (!_fileBody->file.is_open())
        throw server::InternalServerError();
    _fileBody->file.seekg (0, _fileBody->file.end);
    int length = _fileBody->file.tellg();
    _fileBody->file.seekg (0, _fileBody->file.beg);
    _fileBody->offset = 0;
    _fileBody->consumed = 0;
    _fileBody->nBytes = 0;
    return (length);
} 

void Response::setContentLength(int length)
{
    std::stringstream ss;
    ss << length;
    std::string len = ss.str();
    addHeader(std::string("Content-Length"), len);
}

void Response::processPOST(Request &request, location *locationMatch)
{
    if (request.getState() == WAIT)
    {
        // setting upload file path
        std::string _contentFile = locationMatch->GetUpload_dir();
        if ((!_contentFile.empty() && !request.getRequestTarget().empty()) && _contentFile[_contentFile.size() - 1] != '/' && request.getRequestTarget()[0] != '/')
            _contentFile += "/";
        _contentFile += request.getRequestTarget();
        request.setContentFile(_contentFile);

        // changing state for request to start reading body
        request.setState(BODY);
        setProgress(POST_HOLD);
        return ;
    }
    else
    {
        // Unexpected state error
        std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [500] [Internal Server Error] [Unexpected state]";
        webServLog(logMessage, ERROR);
        throw server::InternalServerError();
    }
}

void Response::processDirectoryRequest(Request &request, location *locationMatch, server *serv, std::string &path)
{
    if (!path.empty() && path[path.size() - 1] != '/')
    {
        //TODO: choose host and port of request !!!!
        /*std::string redirectURL = "http://" + serv->Get_host() + ":" + serv->getSock()[0].first + request.getRequestTarget() + "/";*/
        std::string redirectURL = "http://" + request.getHostHeader() + request.getRequestTarget() + "/";
        addHeader(std::string("Location"), redirectURL);
        std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [301] [Moved Permanently] [Redirecting to: " + redirectURL + "]";
        webServLog(logMessage, WARNING);
        return (setHttpResponse(301, "Moved Permanently", *this, serv));
    }

    if (request.getMethod() != "GET")
    {
        std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [405] [Method Not Allowed] [Unsupported method]";
        webServLog(logMessage, WARNING);
        return (setHttpResponse(405, "Method Not Allowed", *this, serv));
    }

    for (size_t i = 0; i < locationMatch->GetIndex().size(); i++)
    {
        std::string dirIndexPath = path + locationMatch->GetIndex()[i];
        FileState indexState = getFileState(dirIndexPath.c_str());
        if (indexState == FILE_IS_REGULAR)
        {
            std::string connection = "close";
            std::string contentType = getMimeType(dirIndexPath);
            setHttpVersion(HTTP_VERSION);
            setStatusCode(200);
            setReasonPhrase("OK");
            addHeader(std::string("Content-Type"), contentType);
            addHeader(std::string("Connection"), connection);
            int length = setFileBody(dirIndexPath);
            setContentLength(length);
            std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [200] [OK] [Index file found]";
            webServLog(logMessage, INFO);
            return ;
        }
    }

    bool directoryListing = locationMatch->GetDirectoryListing();
    if (directoryListing)
    {
        std::string htmlDirectoryListing = listDirectoryHTML(path.c_str());
        if (htmlDirectoryListing.empty())
        {
            std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [500] [Internal Server Error] [Directory listing failed]";
            webServLog(logMessage, ERROR);
            throw server::InternalServerError();
        }
        std::string connection = "close";
        std::string contentType = getMimeType("foo.html");
        setHttpVersion(HTTP_VERSION);
        setStatusCode(200);
        setReasonPhrase("OK");
        addHeader(std::string("Content-Type"), contentType);
        addHeader(std::string("Connection"), connection);
        setTextBody(htmlDirectoryListing);
        setContentLength(htmlDirectoryListing.size());
        std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [200] [OK] [Directory listing]";
        webServLog(logMessage, INFO);
        return ;
    }
    else
    {
        std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [403] [Forbidden] [Directory listing not allowed]";
        webServLog(logMessage, WARNING);
        return (setHttpResponse(403, "Forbidden", *this, serv));
    }
}

void Response::processGET(Request &request, std::string &path)
{
    std::string connection = "close";
    std::string contentType = getMimeType(path);

    setHttpVersion(HTTP_VERSION);
    setStatusCode(200);
    addHeader(std::string("Content-Type"), contentType);
    addHeader(std::string("Connection"), connection);
    setReasonPhrase("OK");
    int length = setFileBody(path);
    setContentLength(length);

    std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [200] [OK] [File found]";
    webServLog(logMessage, INFO);
    return ;
}

void Response::processDELETE(Request &request, server *serv, std::string &path)
{
    if (unlink(path.c_str()) == 0)
    {
        std::string connection = "close";
        std::string contentLength = "0";

        addHeader(std::string("Connection"), connection);
        addHeader(std::string("Content-Length"), contentLength);
        setHttpVersion(HTTP_VERSION);
        setStatusCode(204);
        setReasonPhrase("No Content");

        setTextBody("");
        setContentLength(0);
        
        std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [204] [No Content] [File deleted]";
        webServLog(logMessage, INFO);
        return ;
    }
    else
    {
        if (errno == EACCES || errno == EPERM || errno == EISDIR) // EACCES: permission denied, EPERM: operation not permitted, EISDIR: is a directory
        {
            std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [403] [Forbidden] [Permission denied]";
            webServLog(logMessage, WARNING);
            return (setHttpResponse(403, "Forbidden", *this, serv));
        }
        else
        {
            std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [500] [Internal Server Error] [File deletion failed]";
            webServLog(logMessage, ERROR);
            throw server::InternalServerError();
        }
    }
}

void Response::buildResponse(Request &request, server *serv)
{
    if (!serv)
    {
        std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [500] [Internal Server Error] [Server not found]";
        webServLog(logMessage, ERROR);
        throw server::InternalServerError();
    }

    if (!request.HostHeaderExists())
    {
        std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [400] [Bad Request] [Required Host Header]";
        webServLog(logMessage, ERROR);
        throw Request::badRequest();
    }

    if (request.getRequestTarget() == "/cgi-bin/login.py")
        handleCGI(*this, request);

    if (getProgress() == POST_HOLD)
    {
        if (request.getMethod() != "POST")
        {
            // Unexpected state Error
            std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [500] [Internal Server Error] [Unexpected state]";
            webServLog(logMessage, ERROR);
            throw server::InternalServerError();
        }
        if (request.getState() != DONE) // Still reading body
            return ;
        std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [201] [Created] [POST request]";
        webServLog(logMessage, INFO);
        setProgress(BUILD_RESPONSE);
        return (setHttpResponse(201, "Created", *this, serv));
    }

    // find the location match
    location* locationMatch = getLocationMatch(
                                    request.getRequestTarget(),
                                    serv->GetLocations(),
                                    serv->Get_number_of_location());
    if (locationMatch == nullptr)
    {
        std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [404] [Not Found] [Location not found]";
        webServLog(logMessage, WARNING);
        return (setHttpResponse(404, "Not Found", *this, serv));
    }
    // std::cout << "Matched location: " << locationMatch->GetType_of_location() << std::endl;

    // check if the method is allowed
    if (methodAllowed(request.getMethod(), locationMatch->GetAllowed_methods()) == false)
    {
        std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [405] [Method Not Allowed] [Unsupported method]";
        webServLog(logMessage, WARNING);
        return (setHttpResponse(405, "Method Not Allowed", *this, serv));
    }
    // std::cout << "Method allowed: " << request.getMethod() << std::endl;

    // check for redirection 
    if (!locationMatch->GetRewrite().empty())
    {
        std::string redirectURL = locationMatch->GetRewrite();
        addHeader(std::string("Location"), redirectURL);
        std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [301] [Moved Permanently] [Redirecting to: " + redirectURL + "]";
        webServLog(logMessage, WARNING);
        return (setHttpResponse(301, "Moved Permanently", *this, serv));
    }

    if (request.getMethod() == "POST")
        return (processPOST(request, locationMatch));

    // Construct full requested path
    std::string path = locationMatch->GetRoot_directory();
    if ((!path.empty() && !request.getRequestTarget().empty()) && path[path.size() - 1] != '/' && request.getRequestTarget()[0] != '/')
        path += "/";
    path += request.getRequestTarget();
    // std::cout << "FULL PATH IS: " << path << std::endl;

    FileState fileState = getFileState(path.c_str());
    if (fileState == FILE_DOES_NOT_EXIST)
    {
        std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [404] [Not Found] [File not found]";
        webServLog(logMessage, WARNING);
        return (setHttpResponse(404, "Not Found", *this, serv));
    }
    else if (fileState == FILE_IS_DIRECTORY)
        return (processDirectoryRequest(request, locationMatch, serv, path));
    else if (fileState == FILE_IS_REGULAR)
    {
        if (request.getMethod() == "GET")
            return (processGET(request, path));
        else if (request.getMethod() == "POST")
        {
            // Unexpected state Error, POST request should be handled before this point
            std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [500] [Internal Server Error] [Unexpected state]";
            webServLog(logMessage, ERROR);
            throw server::InternalServerError();
        }
        else if (request.getMethod() == "DELETE")
            return (processDELETE(request, serv, path));
    }
}

void Response::createResponseStream()
{
    if (_response.empty())
    {
        // Build the response line
        std::ostringstream responseStream;
        responseStream << _httpVersion << " " << _statusCode << " " << _reasonPhrase << "\r\n";

        // Add headers
        std::map<std::string, std::string>::const_iterator it;
        for (it = _headers.begin(); it != _headers.end(); it++)
            responseStream << it->first << ": " << it->second << "\r\n";
        responseStream << "\r\n";

        _response = responseStream.str();
    }
    _progress = SEND_HEADERS;
}
