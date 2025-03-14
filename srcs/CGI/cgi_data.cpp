#include "cgi_data.hpp"
#include "Request.hpp"
#include "Response.hpp"

// CGI::CGI(){}

CGI::CGI(){
    signal(SIGPIPE, SIG_IGN);
    _status = CGI_SETUP;
    stdin_pipe[0] = -1;
    stdin_pipe[1] = -1;
    stdout_pipe[0] = -1;
    stdout_pipe[1] = -1;
    _start_time = 0;
    _response.str("");
    scriptPath = "";
    pathInfo = "";
    interpreter = "";
};

enum CGIStatus CGI::getCGIstatus() const { return _status; };

void CGI::setScriptPath(std::string __scriptPath){
    scriptPath = __scriptPath;
}

void CGI::setPathInfo(std::string __pathInfo){
    pathInfo = __pathInfo;
}

bool CGI::pipesNotSet() const
{
    return (stdin_pipe[0] == -1 && stdin_pipe[1] == -1 && stdout_pipe[0] == -1 && stdout_pipe[1] == -1);
}

CGI::~CGI()
{
    closeAllPipes();
    // Kill any remaining process
    if (_pid > 0) {
        kill(_pid, SIGKILL);
        _pid = -1;
    }
}

std::string getInterpreter(const std::string& filePath) {
    typedef std::map<std::string, std::string> InterpreterMap;
    InterpreterMap interpreterMap;

    interpreterMap[".py"] = "/usr/bin/python3"; 
    interpreterMap[".php"] = "/usr/bin/php-cgi";     

    std::string::size_type dotPos = filePath.find_last_of(".");
    if (dotPos == std::string::npos) {
        return ""; 
    }
    std::string extension = filePath.substr(dotPos);

    InterpreterMap::const_iterator it = interpreterMap.find(extension);
    if (it != interpreterMap.end()) {
        return it->second;
    }
    return "";
}

std::map<std::string, std::string> extractHeaders(const std::string& output) {
    std::map<std::string, std::string> headers;
    std::istringstream stream(output);
    std::string line;
    while (std::getline(stream, line) && !line.empty()) {
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);
            headers[key] = value;
        }
    }
    return headers;
}
std::string extractBody(const std::string& output) {
    size_t pos = output.find("\n\n");
    if (pos != std::string::npos) {
        return output.substr(pos + 2);
    }
    return "";
}

std::string convertQueryMapToString(const std::map<std::string, std::string>& query) {
    std::ostringstream stream;
    for (std::map<std::string, std::string>::const_iterator it = query.begin(); it != query.end(); ++it) {
        stream << it->first << "=" << it->second << "&";
    }
    std::string queryString = stream.str();
    if (!queryString.empty()) {
        queryString = queryString.substr(0, queryString.length() - 1);
    }
    return queryString;
}

std::string ScriptPath_PathInfo(std::string& scriptPath, const std::string& requestTarget) {
    
    size_t pyPos = requestTarget.find(".py");
    size_t phpPos = requestTarget.find(".php");


    size_t scriptEnd = std::string::npos;

    if (pyPos != std::string::npos)
        scriptEnd = pyPos + 3;
    else if (phpPos != std::string::npos) 
        scriptEnd = phpPos + 4;
    
    // only `/` or `?` allowed after extension
    if (scriptEnd != std::string::npos && requestTarget[scriptEnd] != '/' && requestTarget[scriptEnd] != '?') {
        scriptEnd = std::string::npos;
    }
    
    if (scriptEnd == std::string::npos) {
        scriptPath += requestTarget;
        return "";
    }
    std::string Info = requestTarget.substr(1, scriptEnd - 1);
    if (scriptPath[scriptPath.length() - 1] != '/')
        scriptPath += "/";
    scriptPath += Info;
    
    std::string pathInfo = "";
    if (scriptEnd < requestTarget.length()) {
        pathInfo = requestTarget.substr(scriptEnd);
    }
    return pathInfo;
}


 
void processSuccessfulResponse(server *serv, Response &response, Request &request, 
                                  const std::string &outputStr) {
    std::map<std::string, std::string> headers = extractHeaders(outputStr);
    std::string responseBody = extractBody(outputStr);
    
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        response.addHeader(it->first, it->second);
        
        if (it->first == "Set-Cookie") {
            std::string cookie = it->second.substr(strlen("session_id= "));
            serv->SetUserToken(cookie);
        }
    }
    
    response.setHttpVersion(HTTP_VERSION);
    response.setStatusCode(200);
    response.setReasonPhrase("OK");
    response.setTextBody(responseBody);
    response.setContentLength(responseBody.size());
    response.setProgress(BUILD_RESPONSE);
    
    std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + 
                           "] [200] [OK] [CGI executed]";
    webServLog(logMessage, INFO);
}

