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

bool isFileValid(const std::string& filePath) {
    return (access(filePath.c_str(), F_OK) == 0);
}

std::string getInterpreter(const std::string& filePath) {
    typedef std::map<std::string, std::string> InterpreterMap;
    InterpreterMap interpreterMap;

    interpreterMap[".py"] = "/usr/bin/python3"; 
    interpreterMap[".sh"] = "/bin/bash";       
    interpreterMap[".php"] = "/usr/bin/php-cgi";     
    interpreterMap[".pl"] = "/usr/bin/perl";     

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
    size_t shpPos = requestTarget.find(".sh");
    size_t plpPos = requestTarget.find(".pl");

    size_t scriptEnd = std::string::npos;

    if (pyPos != std::string::npos)
        scriptEnd = pyPos + 3;
    else if (shpPos != std::string::npos)
        scriptEnd = shpPos + 3;
    else if (plpPos != std::string::npos)
        scriptEnd = plpPos + 3;
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




void setupEnvironment(Request &request, std::map<std::string, std::string> &env, const std::string &pathInfo) {
    env["REQUEST_METHOD"] = request.getMethod();
    env["QUERY_STRING"] = convertQueryMapToString(request.getQuery());
    env["CONTENT_TYPE"] = request.getHeader("Content-Type");
    env["CONTENT_LENGTH"] = request.getHeader("Content-Length");
    env["HTTP_COOKIE"] = request.getHeader("Cookie");
    if (!pathInfo.empty())
        env["PATH_INFO"] = pathInfo;

    const std::map<std::string, std::string>& headers = request.getHeaders();
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        std::string envVar = "HTTP_" + it->first;
        
        for (std::string::iterator charIt = envVar.begin(); charIt != envVar.end(); ++charIt) {
            if (*charIt == '-') {
                *charIt = '_';
            }
        }
        
        for (std::string::iterator charIt = envVar.begin(); charIt != envVar.end(); ++charIt) {
            if (*charIt >= 'a' && *charIt <= 'z') {
                *charIt = *charIt - 32;
            }
        }
        
        env[envVar] = it->second;
    }
}

bool validateScriptPath(server *serv, Response &response, Request &request, int *stdin_pipe, std::string &scriptPath) {
    if (!isFileValid(scriptPath.c_str())) {
        close(stdin_pipe[0]);
        close(stdin_pipe[1]);
        // response.setProgress(BUILD_RESPONSE);
        setHttpResponse(404, "Not Found", response, serv);
        return false;
    }

    if (request.getRequestTarget() == "/home.py") {
        if (!isTokenExist(serv->GetUserToken(), request.getHeader("Cookie"))) {
            close(stdin_pipe[0]);
            close(stdin_pipe[1]);
            // response.setProgress(BUILD_RESPONSE);
            setHttpResponse(403, "Forbidden", response, serv);
            return false;
        }
    }
    
    return true;
}

bool setupPipes(int *stdin_pipe, int *stdout_pipe, int *status_pipe, Response &response, Request &request) {
    (void)response;
    if (pipe(stdout_pipe) == -1) {
        close(stdin_pipe[0]);
        close(stdin_pipe[1]);
        std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + 
                               "] [500] [Internal Server Error] [Failed to create stdout pipe]";
        webServLog(logMessage, ERROR);
        // response.setProgress(BUILD_RESPONSE);
        throw server::InternalServerError();
        return false;
    }
    
    if (pipe(status_pipe) == -1) {
        close(stdin_pipe[0]);
        close(stdin_pipe[1]);
        close(stdout_pipe[0]);
        close(stdout_pipe[1]);
        std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + 
                               "] [500] [Internal Server Error] [Failed to create status pipe]";
        webServLog(logMessage, ERROR);
        // response.setProgress(BUILD_RESPONSE);
        throw server::InternalServerError();
        return false;
    }
    
    return true;
}

void handleChildProcess(int *stdin_pipe, int *stdout_pipe, int *status_pipe, 
                          std::map<std::string, std::string> &env, 
                          const std::string &interpreter, 
                          const std::string &scriptPath,
                          Request &request) {
    close(stdin_pipe[1]);
    close(stdout_pipe[0]);
    close(status_pipe[0]);
    
    dup2(stdin_pipe[0], STDIN_FILENO);
    close(stdin_pipe[0]);
    
    dup2(stdout_pipe[1], STDOUT_FILENO);
    close(stdout_pipe[1]);
    
    std::vector<std::string> envStrings;
    std::vector<char*> envp;
    
    for (std::map<std::string, std::string>::const_iterator envIt = env.begin(); envIt != env.end(); ++envIt) {
        std::string envString = envIt->first + "=" + envIt->second;
        envStrings.push_back(envString);
    }
    
    for (std::vector<std::string>::iterator strIt = envStrings.begin(); strIt != envStrings.end(); ++strIt) {
        envp.push_back(const_cast<char*>(strIt->c_str()));
    }
    envp.push_back(NULL);
    
    std::vector<const char*> args;
    
    if (!interpreter.empty()) {
        args.push_back(interpreter.c_str());
    }
    args.push_back(scriptPath.c_str());
    args.push_back(NULL);
    
    execve(args[0], const_cast<char* const*>(&args[0]), &envp[0]);
    
    std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + 
                           "] [500] [Internal Server Error] [Unsupported script type]";
    webServLog(logMessage, ERROR);
    
    char error_msg = 1;
    write(status_pipe[1], &error_msg, 1);
    close(status_pipe[1]);

    std::cerr << "============= irrelevant =============" << std::endl;
    
    exit(3);
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

void handleTimeoutError(pid_t pid, Response &response, Request &request) {
    kill(pid, SIGTERM);
    
    struct timeval grace_period;
    grace_period.tv_sec = 1;
    grace_period.tv_usec = 0;
    select(0, NULL, NULL, NULL, &grace_period);
    
    int status;
    if (waitpid(pid, &status, WNOHANG) == 0) {
        kill(pid, SIGKILL);
        waitpid(pid, NULL, 0);
    }
    
    std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + 
                           "] [504] [Gateway Timeout] [CGI script timed out]";
    webServLog(logMessage, ERROR);
    
    response.setProgress(BUILD_RESPONSE);
    response.setStatusCode(504);
    response.setReasonPhrase("Gateway Timeout");
    response.setHttpVersion(HTTP_VERSION);
    response.setTextBody("CGI process exceeded the time limit.");
    response.setContentLength(strlen("CGI process exceeded the time limit."));
}
