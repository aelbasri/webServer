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

// std::string set_cookie(const std::string& username) {
//     std::string cookie = "Set-Cookie: username=" + username + "; Max-Age=3600";
//     return cookie;
// }

std::string set_cookie(const std::string& name, const std::string& value) {
    std::string cookie =  name + "=" + value + "; Max-Age=3600" + "; Path=/\r\n";
    return cookie;
}


void parseCredentials(const std::string& input, std::string& username, std::string& password, bool& rememberMe) {
    size_t userPos = input.find("username=");
    if (userPos != std::string::npos) {
        size_t userStart = userPos + 9;
        size_t userEnd = input.find('&', userStart);
        username = input.substr(userStart, userEnd - userStart);
    }

    size_t passPos = input.find("password=");
    if (passPos != std::string::npos) {
        size_t passStart = passPos + 9;
        size_t passEnd = input.find('&', passStart);
        password = input.substr(passStart, passEnd - passStart);
    }

    size_t rememberPos = input.find("remember_me=");
    if (rememberPos != std::string::npos) {
        size_t rememberStart = rememberPos + 12;
        size_t rememberEnd = input.find('&', rememberStart);
        std::string rememberValue = input.substr(rememberStart, rememberEnd - rememberStart);
        rememberMe = (rememberValue == "on");
    } else {
        rememberMe = false;
    }
}
bool isRememberMeOn(const std::string& input) {
    size_t rememberPos = input.find("remember_me=");
    if (rememberPos == std::string::npos) {
        return false; 
    }

    size_t valueStart = rememberPos + 12;
    size_t valueEnd = input.find('&', valueStart);

    std::string rememberValue;
    if (valueEnd == std::string::npos) {
        rememberValue = input.substr(valueStart);
    } else {
        rememberValue = input.substr(valueStart, valueEnd - valueStart);
    }
    return (rememberValue == "on");
}

void Response::buildResponse(Request &request, server *serv)
{
    std::string username, password;
    bool remember_me;


    if (request.getRequestTarget() == "/cgi-bin/login.py"){
        std::string s;
        CGI _cgi("./cgi-bin/login.py", "/usr/bin/python3");
        
        std::ifstream myfile;
        myfile.open("/tmp/.contentData");
        while (getline(myfile, s))
        {
            s += s;
            if (!s.empty())
                s.push_back('\n');
        }
        std::string executable = _cgi.RunCgi(s);
        parseCredentials(s, username, password, remember_me);
        std::string cookie = set_cookie("username", password);
        if (remember_me == true)
            addHeader(std::string("Set-Cookie"), cookie);
        std::cout << "the autput of cgi is : |" << executable <<"|" <<  std::endl;
    }
    if (!serv)
    {
        std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [500] [Internal Server Error] [Server not found]";
        webServLog(logMessage, ERROR);
        throw server::InternalServerError();
    }

    if (getProgress() == POST_HOLD)
    {
        if (request.getMethod() != "POST")
        {
            // Unexpected state Error
            std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [500] [Internal Server Error] [Unexpected state]";
            webServLog(logMessage, ERROR);
            throw server::InternalServerError();
        }
        // TODO: check max body size
        if (request.getState() != DONE)
        {
            std::cout << "Still waiting for body" << std::endl;
            return ;
        }
        std::cout << "Body received" << std::endl;
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
        webServLog(logMessage, ERROR);
        return (setHttpResponse(404, "Not Found", *this, serv));
    }
    // std::cout << "Matched location: " << locationMatch->GetType_of_location() << std::endl;

    // check if the method is allowed
    if (methodAllowed(request.getMethod(), locationMatch->GetAllowed_methods()) == false)
    {
        std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [405] [Method Not Allowed] [Unsupported method]";
        webServLog(logMessage, ERROR);
        return (setHttpResponse(405, "Method Not Allowed", *this, serv));
    }
        // std::cout << "Method allowed: " << request.getMethod() << std::endl;

    // check for redirection 
    if (!locationMatch->GetRewrite().empty())
    {
        std::string redirectURL = locationMatch->GetRewrite();
        addHeader(std::string("Location"), redirectURL);
        std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [301] [Moved Permanently] [Redirecting to: " + redirectURL + "]";
        webServLog(logMessage, INFO);
        return (setHttpResponse(301, "Moved Permanently", *this, serv));
    }


    // handle POST request
    if (request.getMethod() == "POST")
    {
        // TODO: UPLOAD TO UPLOAD_DIR, Check filename if already exists
        if (request.getState() == WAIT)
        {
            // std::cout << "Swich state to BODY state" << std::endl;
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

    // Construct full requested path
    std::string path = locationMatch->GetRoot_directory();
    if ((!path.empty() && !request.getRequestTarget().empty()) && path[path.size() - 1] != '/' && request.getRequestTarget()[0] != '/')
        path += "/";
    path += request.getRequestTarget();

    // if directory, if path not ends with '/` redirect to path/ , then check if there is index, else directory listing
    // else, regular file handling
    FileState fileState = getFileState(path.c_str());
    if (fileState == FILE_DOES_NOT_EXIST)
    {
        std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [404] [Not Found] [File not found]";
        webServLog(logMessage, ERROR);
        return (setHttpResponse(404, "Not Found", *this, serv));
    }
    else if (fileState == FILE_IS_DIRECTORY)
    {
        if (!path.empty() && path[path.size() - 1] != '/')
        {
            // choose host and port of request !!!!
            std::string redirectURL = "http://" + serv->Get_host() + ":" + serv->getSock()[0].first + request.getRequestTarget() + "/";
            addHeader(std::string("Location"), redirectURL);
            std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [301] [Moved Permanently] [Redirecting to: " + redirectURL + "]";
            webServLog(logMessage, INFO);
            return (setHttpResponse(301, "Moved Permanently", *this, serv));
        }

        if (request.getMethod() != "GET")
        {
            std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [405] [Method Not Allowed] [Unsupported method]";
            webServLog(logMessage, ERROR);
            return (setHttpResponse(405, "Method Not Allowed", *this, serv));
        }
        
        //TODO: whili 3lihoum
        if (locationMatch->GetIndex().size() > 0 && !locationMatch->GetIndex()[0].empty())
        {
            // TODO: WHIli 3lihoum
            std::string dirIndexPath = path + locationMatch->GetIndex()[0];
            FileState indexState = getFileState(dirIndexPath.c_str());
            if (indexState == FILE_IS_REGULAR)
            {
                std::string connection = "close";
                std::string contentType = getMimeType(dirIndexPath);

                setHttpVersion(HTTP_VERSION);
                setStatusCode(200);
                setReasonPhrase("OK");
                setFile(dirIndexPath);
                setContentLength();
                addHeader(std::string("Content-Type"), contentType);
                addHeader(std::string("Connection"), connection);
                std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [200] [OK] [Index file found]";
                webServLog(logMessage, INFO);
                return ;
            }
            else
            {
                std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [403] [Forbidden] [Index file not found]";
                webServLog(logMessage, ERROR);
                return (setHttpResponse(403, "Forbidden", *this, serv));
            }
        }
        bool directoryListing = locationMatch->GetDirectoryListing();
        if (directoryListing)
        {
            std::string htmlDirectoryListing = listDirectoryHTML(path.c_str());
            if (htmlDirectoryListing.empty()) // if empty, handle error (probably syscall failed) 500 response
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
            setTextBody(htmlDirectoryListing);
            setContentLength();
            addHeader(std::string("Content-Type"), contentType);
            addHeader(std::string("Connection"), connection);
            std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [200] [OK] [Directory listing]";
            webServLog(logMessage, INFO);
            return ;
        }
        else
        {
            std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [403] [Forbidden] [Directory listing not allowed]";
            webServLog(logMessage, ERROR);
            return (setHttpResponse(403, "Forbidden", *this, serv));
        }
    }
    else if (fileState == FILE_IS_REGULAR)
    {
        if (request.getMethod() == "GET")
        {
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
            std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [200] [OK] [File found]";
            webServLog(logMessage, INFO);
            return ;
        }
        else if (request.getMethod() == "POST")
        {
            // Unexpected state Error, POST request should be handled before this point
            std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [500] [Internal Server Error] [Unexpected state]";
            webServLog(logMessage, ERROR);
            throw server::InternalServerError();
        }
        else if (request.getMethod() == "DELETE")
        {
            // std::cout << "DELETing file: " << path << std::endl;
            if (unlink(path.c_str()) == 0)
            {
                std::string connection = "close";
                std::string contentLength = "0";

                addHeader(std::string("Connection"), connection);
                addHeader(std::string("Content-Length"), contentLength);
                setHttpVersion(HTTP_VERSION);
                setStatusCode(204);
                setReasonPhrase("No Content");
                std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [204] [No Content] [File deleted]";
                webServLog(logMessage, INFO);
                return ;
            }
            else
            {
                if (errno == EACCES || errno == EPERM || errno == EISDIR) // EACCES: permission denied, EPERM: operation not permitted, EISDIR: is a directory
                {
                    std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [403] [Forbidden] [Permission denied]";
                    webServLog(logMessage, ERROR);
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
}
