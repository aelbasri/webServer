

#include "cgi_data.hpp"

CGI::CGI(){}

CGI::CGI(std::string __path): _path(__path){} 

CGI::~CGI(){}

void CGI::SetPath(std::string __path){
	_path = __path;
}

std::string CGI::GetPath() const{
	return (_path);
}

int CGI::GetExitStatus() const{
    return _ExitStatus;
}

bool isFileValid(const std::string& filePath) {
    return (access(filePath.c_str(), F_OK) == 0);
}

std::string getInterpreter(const std::string& filePath) {
    typedef std::map<std::string, std::string> InterpreterMap;
    InterpreterMap interpreterMap;

    interpreterMap[".py"] = "/usr/bin/python3"; 
    interpreterMap[".sh"] = "/bin/bash";       
    interpreterMap[".php"] = "/usr/bin/php";     
    interpreterMap[".pl"] = "/usr/bin/perl";     
    interpreterMap[".rb"] = "/usr/bin/ruby";     

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


#define CGI_TIMEMOUT_SECONDS 2

void CGI::RunCgi(server *serv, Response &response, Request &request) {


    int *stdin_pipe = response.getCGIPIPE();
    if (stdin_pipe[0] == -1 && stdin_pipe[1] == -1)
    {
        if (pipe(stdin_pipe) == -1) {
            std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [500] [Internal Server Error] [Failed to create pipe]";
            webServLog(logMessage, ERROR);
            response.setProgress(BUILD_RESPONSE);
            throw server::InternalServerError();
        }
    }

    if (request.getMethod() == "POST") {
        if (request.getState() == WAIT)
        {
            request.setWriteInPipe(true);
            request.setFd(stdin_pipe[1]);
            request.setState(BODY);
            response.setProgress(POST_HOLD);
            request.handle_request(request.getBuffer());
            return;
        }
    }

    std::string scriptPath = ".";
    std::string pathInfo = ScriptPath_PathInfo(scriptPath, request.getRequestTarget());
    
    if (!isFileValid(scriptPath.c_str()))
    {
        close(stdin_pipe[0]);
        close(stdin_pipe[1]);
        response.setProgress(BUILD_RESPONSE);
        return (setHttpResponse(404, "Not Found", response, serv));
    }

    if (scriptPath == "./cgi-bin/home.py")
    {
        if (!isTokenExist(serv->GetUserToken(), request.getHeader("Cookie"))){
            close(stdin_pipe[0]);
            close(stdin_pipe[1]);
            response.setProgress(BUILD_RESPONSE);
            return (setHttpResponse(403, "Forbidden", response, serv));
        }
    }
        
    std::string interpreter = getInterpreter(scriptPath);
    if (interpreter.empty())
    {
        close(stdin_pipe[0]);
        close(stdin_pipe[1]);
        response.setProgress(BUILD_RESPONSE);
        return (setHttpResponse(404, "Not Found", response, serv));
    }

    std::map<std::string, std::string> env;
    env["REQUEST_METHOD"] = request.getMethod();
    env["QUERY_STRING"] = convertQueryMapToString(request.getQuery());
    env["CONTENT_TYPE"] = request.getHeader("Content-Type");
    env["CONTENT_LENGTH"] = request.getHeader("Content-Length");
    env["HTTP_COOKIE"] = request.getHeader("Cookie");
    if (!pathInfo.empty())
        env["PATH_INFO"] = pathInfo;

    for (const auto& header : request.getHeaders()) {
        std::string envVar = "HTTP_" + header.first;
        std::replace(envVar.begin(), envVar.end(), '-', '_');
        std::transform(envVar.begin(), envVar.end(), envVar.begin(), ::toupper);
        env[envVar] = header.second;
    }

    int stdout_pipe[2];
    if (pipe(stdout_pipe) == -1) {
        close(stdin_pipe[0]);
        close(stdin_pipe[1]);
        std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [500] [Internal Server Error] [Failed to create pipe]";
        webServLog(logMessage, ERROR);
        response.setProgress(BUILD_RESPONSE);
        throw server::InternalServerError();
    }

    pid_t pid = fork();
    if (pid == -1) {
        close(stdin_pipe[0]);
        close(stdin_pipe[1]);
        close(stdout_pipe[0]);
        close(stdout_pipe[1]);

        std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [500] [Internal Server Error] [Failed to fork]";
        webServLog(logMessage, ERROR);
        response.setProgress(BUILD_RESPONSE);
        throw server::InternalServerError();
    }

    if (pid == 0) {
        // Child process
        close(stdin_pipe[1]);
        close(stdout_pipe[0]);

        dup2(stdin_pipe[0], STDIN_FILENO);
        close(stdin_pipe[0]);

        dup2(stdout_pipe[1], STDOUT_FILENO);
        close(stdout_pipe[1]);

        std::vector<std::string> envStrings;
        std::vector<char*> envp;
        
        std::map<std::string, std::string>::const_iterator envIt;
        for (envIt = env.begin(); envIt != env.end(); ++envIt) {
            std::string envString = envIt->first + "=" + envIt->second;
            envStrings.push_back(envString);
        }

        std::vector<const char*> args;
        
        if (!interpreter.empty()) {
            args.push_back(interpreter.c_str());
        }
        args.push_back(scriptPath.c_str());
        args.push_back(NULL);

        execv(args[0], const_cast<char* const*>(&args[0]));
        
        exit(1);
    } 
    else {
        // Parent process
        close(stdin_pipe[0]);
        close(stdin_pipe[1]);
        close(stdout_pipe[1]);

        // Set up the timeout using select
        fd_set read_fds;
        struct timeval tv;
        
        // Initialize file descriptor sets
        FD_ZERO(&read_fds);
        FD_SET(stdout_pipe[0], &read_fds);
        
        // Set timeout value
        tv.tv_sec = CGI_TIMEMOUT_SECONDS;
        tv.tv_usec = 0;
        
        // Find the highest file descriptor
        int max_fd = stdout_pipe[0];
        
        // Wait for output from CGI script or timeout
        int select_result = select(max_fd + 1, &read_fds, NULL, NULL, &tv);
        
        if (select_result == -1) {
            // Select error
            close(stdout_pipe[0]);
            kill(pid, SIGKILL);
            waitpid(pid, NULL, 0);
            
            std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [500] [Internal Server Error] [Select error]";
            webServLog(logMessage, ERROR);
            response.setProgress(BUILD_RESPONSE);
            throw server::InternalServerError();
        } 
        else if (select_result == 0) {
            // Timeout occurred
            close(stdout_pipe[0]);
            
            // First try SIGTERM for graceful termination
            kill(pid, SIGTERM);
            
            // Give the process a chance to terminate gracefully
            struct timeval grace_period;
            grace_period.tv_sec = 1;
            grace_period.tv_usec = 0;
            select(0, NULL, NULL, NULL, &grace_period);
            
            // Check if process is still running, if so, use SIGKILL
            int status;
            if (waitpid(pid, &status, WNOHANG) == 0) {
                kill(pid, SIGKILL);
                waitpid(pid, NULL, 0);
            }
            
            std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [504] [Gateway Timeout] [CGI script timed out]";
            webServLog(logMessage, ERROR);
            response.setProgress(BUILD_RESPONSE);
            response.setStatusCode(504);
            response.setReasonPhrase("Gateway Timeout");
            response.setHttpVersion(HTTP_VERSION);
            response.setTextBody("CGI process exceeded the time limit.");
            response.setContentLength(strlen("CGI process exceeded the time limit."));
            return;
        } 
        else {
            // Read stdout from the CGI script
            std::ostringstream output;
            char buffer[1024];
            ssize_t bytesRead;
            
            // Only read if stdout pipe is ready
            if (FD_ISSET(stdout_pipe[0], &read_fds)) {
                while ((bytesRead = read(stdout_pipe[0], buffer, sizeof(buffer))) > 0) {
                    output.write(buffer, bytesRead);
                }
            }
            
            close(stdout_pipe[0]);
            
            int status;
            waitpid(pid, &status, 0);

            if (WIFEXITED(status)) {
                if (WEXITSTATUS(status) == 0 || WEXITSTATUS(status) == 1)
                {
                    // Extract headers and body from the output
                    std::map<std::string, std::string> headers = extractHeaders(output.str());
                    std::string responseBody = extractBody(output.str());
                    for (const auto& header : headers) {
                        response.addHeader(header.first, header.second);
                        if (header.first == "Set-Cookie")
                        {
                            std::string cookie = header.second.substr(strlen("session_id= "));
                            serv->SetUserToken(cookie);
                        }
                    }
                    response.setHttpVersion(HTTP_VERSION);
                    response.setStatusCode(200);
                    response.setReasonPhrase("OK");
                    response.setTextBody(responseBody);
                    int length = responseBody.size();
                    response.setContentLength(length);
                    response.setProgress(BUILD_RESPONSE);
                    std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [200] [OK] [CGI executed]";
                    webServLog(logMessage, INFO);
                    return;
                }
            }
            std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [500] [Internal Server Error] [CGI script failed]";
            webServLog(logMessage, ERROR);
            response.setProgress(BUILD_RESPONSE);
            throw server::InternalServerError();
        }
    } 
    return;
}
