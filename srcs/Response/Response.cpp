#include "Response.hpp"
#include "Connection.hpp"
#include "location.hpp"
#include <clocale>
#include <cmath>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "log.hpp"

Response::Response()
{
    _headerStream = "";
    _totalBytesSent = 0;
    _sent = false;
    _progress = BUILD_RESPONSE;
    _fileBody = nullptr;
    _isFile = true;
    _CGIPIPE[0] = -1;
    _CGIPIPE[1] = -1;

    _cgi = nullptr;
}

Response::~Response()
{
    if (_fileBody)
    {
        delete _fileBody;
        _fileBody = nullptr;
    }

    if (_cgi)
    {
        delete _cgi;
        _cgi = nullptr;
    }
}

// Getters
enum Progress Response::getProgress() const { return _progress; };
std::string Response::getHeaderStream() const { return _headerStream; };
responseBodyFile *Response::getFileBody() { return _fileBody; };
std::string Response::getTextBody() const { return _textBody; };
size_t Response::getTotalBytesSent() const { return _totalBytesSent; };
int Response::getStatusCode() const { return _statusCode; };
int Response::getSocket() const { return _sock; };
int *Response::getCGIPIPE() { return _CGIPIPE; };
bool Response::getIsFile() const {return _isFile ;};

// Setters
void Response::setTotalBytesSent(size_t bytes) { _totalBytesSent = bytes; };
void Response::setSent(bool sent) { _sent = sent; };
void Response::setProgress(enum Progress progress) { _progress = progress; };
void Response::setSocket(int socket) { _sock = socket; };
void Response::setCGIPIPE(int pipe[2]) { _CGIPIPE[0] = pipe[0]; _CGIPIPE[1] = pipe[1]; };
void Response::setHttpVersion(const std::string &version) { _httpVersion = version; }
void Response::setStatusCode(int status) {_statusCode = status;}
void Response::setReasonPhrase(const std::string &phrase) {_reasonPhrase = phrase;}
void Response::addHeader(const std::string &key, const std::string &value) {_headers[key] = value;}
void Response::setTextBody(const std::string &body) { _textBody = body; _isFile = false; }
int Response::setFileBody(std::string path) {
    _fileBody = new responseBodyFile();
    _fileBody->file.open(path.c_str());
    if (!_fileBody->file.is_open())
        throw server::InternalServerError();
    _fileBody->file.seekg (0, _fileBody->file.end);
    int length = _fileBody->file.tellg();
    _fileBody->fileSize = length;
    _fileBody->file.seekg (0, _fileBody->file.beg);
    _fileBody->offset = 0;
    _fileBody->consumed = 0;
    _fileBody->nBytes = 0;
    _isFile  = true;
    return (length);
} 
void Response::setContentLength(int length)
{
    if (length > CONTENT_LENGTH_LIMIT && getTextBody().empty())
        return (addHeader(std::string("Transfer-Encoding"), std::string("chunked")));
    std::stringstream ss;
    ss << length;
    std::string len = ss.str();
    addHeader(std::string("Content-Length"), len);
}

void Response::processPOST(Request &request, location *locationMatch)
{
    (void)locationMatch;
    if (request.getState() == WAIT)
    {
        // changing state for request to start reading body
        request.setState(BODY);
        setProgress(POST_HOLD);
        // process remaining body in buffer
        request.handle_request(request.getBuffer());
        return ;
    }
    else
    {
        // Unexpected state error
        std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [500] [Internal Server Error] [Unexpected statefefee]";
        webServLog(logMessage, ERROR);
        throw server::InternalServerError();
    }
}

void Response::processDirectoryRequest(Request &request, location *locationMatch, server *serv, std::string &path)
{
    if (!path.empty() && path[path.size() - 1] != '/')
    {
        std::string redirectURL = "http://" + request.getHeader("Host") + request.getRequestTarget() + "/";
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
            std::string contentType = getMimeType(dirIndexPath);
            setHttpVersion(HTTP_VERSION);
            setStatusCode(200);
            setReasonPhrase("OK");
            addHeader(std::string("Content-Type"), contentType);
            int length = setFileBody(dirIndexPath);
            setContentLength(length);
            std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [200] [OK] [Index file found]";
            webServLog(logMessage, INFO);
            setProgress(CREATE_HEADERS_STREAM);
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
        std::string contentType = getMimeType("foo.html");
        setHttpVersion(HTTP_VERSION);
        setStatusCode(200);
        setReasonPhrase("OK");
        addHeader(std::string("Content-Type"), contentType);
        setTextBody(htmlDirectoryListing);
        setContentLength(htmlDirectoryListing.size());
        std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [200] [OK] [Directory listing]";
        webServLog(logMessage, INFO);
        setProgress(CREATE_HEADERS_STREAM);
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
    std::string contentType = getMimeType(path);
    setHttpVersion(HTTP_VERSION);
    setStatusCode(200);
    addHeader(std::string("Content-Type"), contentType);
    setReasonPhrase("OK");
    int length = setFileBody(path);
    setContentLength(length);
    std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [200] [OK] [File found]";
    webServLog(logMessage, INFO);
    setProgress(CREATE_HEADERS_STREAM);
    return ;
}

void Response::processDELETE(Request &request, server *serv, std::string &path)
{
    if (std::remove(path.c_str()) == 0)
    {
        std::string contentLength = "0";
        addHeader(std::string("Content-Length"), contentLength);
        setHttpVersion(HTTP_VERSION);
        setStatusCode(204);
        setReasonPhrase("No Content");
        setTextBody("");
        setContentLength(0);
        std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [204] [No Content] [File deleted]";
        webServLog(logMessage, INFO);
        setProgress(CREATE_HEADERS_STREAM);
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

    if (request.getHeader("Host").empty())
    {
        std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [400] [Bad Request] [Required Host Header]";
        webServLog(logMessage, ERROR);
        throw Request::badRequest();
    }

    /*if (isCgiPath(request.getRequestTarget()))*/
    /*    return (handleCGI2(serv, *this, request));*/

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
        std::string filename = request.getContentFile();
        addHeader(std::string("File-Name"), filename.substr(filename.find_last_of("/") + 1));
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

    if (methodAllowed(request.getMethod(), locationMatch->GetAllowed_methods()) == false)
    {
        std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [405] [Method Not Allowed] [Unsupported method]";
        webServLog(logMessage, WARNING);
        return (setHttpResponse(405, "Method Not Allowed", *this, serv));
    }

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

void Response::createResponseStream(std::string &connectionHeader)
{
    if (_progress != CREATE_HEADERS_STREAM)
        return ;
    if (_headerStream.empty())
    {
        // Build the response line
        std::ostringstream responseStream;
        responseStream << _httpVersion << " " << _statusCode << " " << _reasonPhrase << "\r\n";

        // Add headers
        responseStream << "Server: " << SERVER_NAME << "\r\n";
        responseStream << "Date: " << getDate() << "\r\n";
        std::map<std::string, std::string>::const_iterator it;
        for (it = _headers.begin(); it != _headers.end(); it++)
        {
            if (it->first != "Connection")
                responseStream << it->first << ": " << it->second << "\r\n";
        }
        if (_statusCode >= 400 || connectionHeader == "close")
            responseStream << "Connection: close" << "\r\n";
        else
            responseStream << "Connection: keep-alive" << "\r\n"; 
        responseStream << "\r\n";

        _headerStream = responseStream.str();
    }
    _progress = SEND_HEADERS;
}
