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
    // exit(10);
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
        parseCredentials(s, username, password, remember_me);
        std::string executable = _cgi.RunCgi(s);
        std::cout << "hey im here ******************* " << executable << std::endl;
        std::string cookie = set_cookie(username, password);
        addHeader(std::string("Set-Cookie"), cookie);

    }
    if (!serv)
        throw server::InternalServerError();

    // find the location match
    location* locationMatch = getLocationMatch(
                                    request.getRequestTarget(),
                                    serv->GetLocations(),
                                    serv->Get_number_of_location());
    if (locationMatch == nullptr)
        return (setError(404, "Not Found", *this, serv));
    // std::cout << "Matched location: " << locationMatch->GetType_of_location() << std::endl;

    // check for redirection 
    if (!locationMatch->GetRewrite().empty())
    {
        std::string redirectURL = locationMatch->GetRewrite();
        addHeader(std::string("Location"), redirectURL);
        return (setError(301, "Moved Permanently", *this, serv));
    }

    // check if the file exists
    std::string path = locationMatch->GetRoot_directory();
    if ((!path.empty() && !request.getRequestTarget().empty()) && path[path.size() - 1] != '/' && request.getRequestTarget()[0] != '/')
        path += "/";
    path += request.getRequestTarget();

    // if directory, if path not ends with '/` redirect to path/ , then check if there is index, else directory listing
    // else, regular file handling
    FileState fileState = getFileState(path.c_str());
    if (fileState == FILE_DOES_NOT_EXIST)
        return (setError(404, "Not Found", *this, serv));
    else if (fileState == FILE_IS_DIRECTORY)
    {
        if (!path.empty() && path[path.size() - 1] != '/')
        {
            // choose host and port of request !!!!
            std::string redirectURL = "http://" + serv->Get_host() + ":" + serv->Get_port()[0] + request.getRequestTarget() + "/";
            std::cout << "Redirect to " << redirectURL << std::endl;
            addHeader(std::string("Location"), redirectURL);
            return (setError(301, "Moved Permanently", *this, serv));
        }
        if (!locationMatch->GetIndex().empty())
        {
            std::string dirIndexPath = path + locationMatch->GetIndex();
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
                return ;
            }
            // else: if cant open file, return 403 (I GUESS !!)
        }
        bool directoryListing = true; // this should come from config file
        if (directoryListing)
        {
            std::string htmlDirectoryListing = listDirectoryHTML(path.c_str());
            if (htmlDirectoryListing.empty()) // if empty, handle error (probably syscall failed) 500 response
                throw server::InternalServerError();
            
            std::string connection = "close";
            std::string contentType = getMimeType("foo.html");

            setHttpVersion(HTTP_VERSION);
            setStatusCode(200);
            setReasonPhrase("OK");
            setTextBody(htmlDirectoryListing);
            setContentLength();
            addHeader(std::string("Content-Type"), contentType);
            addHeader(std::string("Connection"), connection);
            return ;
        }
    }
    else if (fileState == FILE_IS_REGULAR)
    {
        // check if the method is allowed
        if (methodAllowed(request.getMethod(), locationMatch->GetAllowed_methods()) == false)
            return (setError(405, "Method Not Allowed", *this, serv));
        // std::cout << "Method allowed: " << request.getMethod() << std::endl;

        // generate response
        std::string connection = "close";
        std::string contentType = getMimeType(path);

        setHttpVersion(HTTP_VERSION);
        setStatusCode(200);
        setReasonPhrase("OK");
        setFile(path);

        setContentLength();
        addHeader(std::string("Content-Type"), contentType);
        // addHeader(std::string("Set-Cookie"), std::string("username=sdcsdc"));
        // addHeader(std::string("Set-Cookie"), std::string("password=tabonmo"));
        addHeader(std::string("Connection"), connection);
        return ;
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
