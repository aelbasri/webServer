#include "cgi_data.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include <unistd.h>

std::string ScriptPath_PathInfo(std::string& scriptPath, const std::string& requestTarget);
std::string getInterpreter(const std::string& filePath);
std::string convertQueryMapToString(const std::map<std::string, std::string>& query);
std::map<std::string, std::string> extractHeaders(const std::string& output);
std::string extractBody(const std::string& output);

void CGI::forkChild(server *serv, Response &response, Request &request)
{
    (void)serv;
    (void)response;
    // Fork a child process
    _pid = fork();
    
    if (_pid < 0)
    {
        // Fork failed
        std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [500] [Internal Server Error] [Failed to fork process]";
        webServLog(logMessage, ERROR);
        closeAllPipes();
        throw server::InternalServerError();
    }
    else if (_pid == 0)
    {
        // Child process
        dup2(stdin_pipe[0], STDIN_FILENO);
        dup2(stdout_pipe[1], STDOUT_FILENO);
        
        // Close unused pipe ends
        close(stdin_pipe[1]);
        close(stdout_pipe[0]);

        std::map<std::string, std::string> env;
        env["REQUEST_METHOD"] = request.getMethod();
        env["QUERY_STRING"] = convertQueryMapToString(request.getQuery());
        env["CONTENT_TYPE"] = request.getHeader("Content-Type");
        env["CONTENT_LENGTH"] = request.getHeader("Content-Length");
        env["HTTP_COOKIE"] = request.getHeader("Cookie");
        if (!pathInfo.empty())
            env["PATH_INFO"] = pathInfo;

            // Required CGI variables
        env["GATEWAY_INTERFACE"] = "CGI/1.1";
        env["SERVER_PROTOCOL"] = "HTTP/1.1";
        env["REDIRECT_STATUS"] = "200"; // Required specifically for PHP-CGI to work properly

        // Server information
        env["SERVER_NAME"] = request.getHeader("Host"); // Or use your server's configured hostname
        env["SERVER_PORT"] = serv->GetPort(response.getSocket());
        env["SERVER_SOFTWARE"] = "WebServer/1.0"; // Replace with your server name/version

        // Script information using your variables
        env["SCRIPT_FILENAME"] = scriptPath;
        // Extract SCRIPT_NAME from request.getRequestTarget() by removing PATH_INFO
        std::string scriptName = request.getRequestTarget();
        if (!pathInfo.empty()) {
            size_t pos = scriptName.find(pathInfo);
            if (pos != std::string::npos) {
                scriptName = scriptName.substr(0, pos);
            }
        }
        env["SCRIPT_NAME"] = scriptName;
        env["DOCUMENT_ROOT"] = serv->GetCgi();
        env["REQUEST_URI"] = request.getRequestTarget();
        env["PHP_SELF"] = scriptName;

        // Client information
        // env["REMOTE_ADDR"] = clientIp;
        // env["REMOTE_PORT"] = clientPort;

        const std::map<std::string, std::string>& headers = request.getHeaders();
        std::map<std::string, std::string>::const_iterator it;
        for (it = headers.begin(); it != headers.end(); ++it) {
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

        std::vector<std::string> envStrings;
        std::vector<char*> envp;
        
        std::map<std::string, std::string>::const_iterator envIt;
        for (envIt = env.begin(); envIt != env.end(); ++envIt) {
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

        exit(EXIT_FAILURE);
    }
    
    // Parent process
    // Close unused pipe ends
    close(stdin_pipe[0]);
    close(stdout_pipe[1]);
    
    // Set start time for timeout checking
    _start_time = time(NULL);
    
    // Update status
    _status = CGI_RUNNING;
}

void CGI::setupCGI(server *serv, Response &response, Request &request)
{
 
    // Check if the CGI script exists
    std::string requestTarget = request.getRequestTarget();
    std::string _scriptPath = serv->GetCgi();
    std::string _pathInfo = ScriptPath_PathInfo(_scriptPath, request.getRequestTarget());

    setScriptPath(_scriptPath);
    setPathInfo(_pathInfo);
    
    // Check if file exists and is executable
    if (access(scriptPath.c_str(), F_OK) != 0)
    {
        std::string logMessage = "[" + request.getMethod() + "] [" + requestTarget + "] [404] [Not Found] [CGI script not found or not executable]";
        webServLog(logMessage, ERROR);
        return (setHttpResponse(404, "Not Found", response, serv));
    }
    
    // Special handling for /home.py
    if (requestTarget == "/home.py")
    {
        if (!isTokenExist(serv->GetUserToken(), request.getHeader("Cookie")))
        {
            close(stdin_pipe[0]);
            close(stdin_pipe[1]);
            std::string logMessage = "[" + request.getMethod() + "] [" + requestTarget + "] [403] [Forbidden] [User not logged in]";
            webServLog(logMessage, WARNING);
            return (setHttpResponse(403, "Forbidden", response, serv));
        }
    }

    // Get the interpreter for the script
    interpreter = getInterpreter(scriptPath);
    if (interpreter.empty())
    {
        return (setHttpResponse(404, "Not Found", response, serv));
    }
    
    if (pipesNotSet())
    {
        // Create pipes for stdin, stdout
        if (pipe(stdin_pipe) < 0 || pipe(stdout_pipe) < 0)
        {
            std::string logMessage = "[" + request.getMethod() + "] [" + requestTarget + "] [500] [Internal Server Error] [Failed to create pipes]";
            webServLog(logMessage, ERROR);
            // response.setProgress(BUILD_RESPONSE);
            throw server::InternalServerError();
        }
    }
    
    // Set the pipes to non-blocking mode
    // fcntl(stdin_pipe[0], F_SETFL, O_NONBLOCK);
    fcntl(stdin_pipe[1], F_SETFL, O_NONBLOCK);
    fcntl(stdout_pipe[0], F_SETFL, O_NONBLOCK);
    // fcntl(stdout_pipe[1], F_SETFL, O_NONBLOCK);
    
    // If it's a POST request and we need to write the body to the CGI script
    if (request.getMethod() == "POST" && request.getState() == WAIT)
    {
        request.setWriteInPipe(true);
        request.setFd(stdin_pipe[1]);
        request.setState(BODY);
        response.setProgress(POST_HOLD);
        request.handle_request(request.getBuffer());
        _status = CGI_BODY_READING;
        return;
    }
    _status = CGI_FORKING;
    return ;
}

bool CGI::processTimedOut() const
{
    // Check for timeout (e.g., 30 seconds)
    time_t current_time = time(NULL);
    if (current_time - _start_time >= CGI_TIMEMOUT_SECONDS)
    {
        // Kill the CGI process if it's still running
        kill(_pid, SIGTERM);
        
        // Wait a moment and forcefully kill if still running
        usleep(1000);
        kill(_pid, SIGKILL);
        
        // Clean up
        close(stdout_pipe[0]);
        
        return (true);
    }

    // std::cout << "CGI NOT TIMED OUT YET" << std::endl;
    return (false);
}

void CGI::closeAllPipes()
{
    close(stdin_pipe[0]);
    close(stdin_pipe[1]);
    close(stdout_pipe[0]);
    close(stdout_pipe[1]);
}

void CGI::RunCgi(server *serv, Response &response, Request &request)
{
    if (_status == CGI_SETUP) {
        setupCGI(serv, response, request);
        return;
    }

    if (_status == CGI_BODY_READING) {
        // std::cout << "READING BODY" << std::endl;
        if (request.getState() == WAIT) {
            return;
        }
        close(stdin_pipe[1]);
        _status = CGI_FORKING;
    }

    if (_status == CGI_FORKING) {
        forkChild(serv, response, request);
        return;
    }
    
    if (_status != CGI_RUNNING) {
        return; // Nothing to do if not in setup or running state
    }
    // std::cout << "RUNNING CGI" << std::endl;
    
    // Check for timeout
    if (processTimedOut()) {
        std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [504] [Gateway Timeout] [CGI script timed out]";
        webServLog(logMessage, ERROR);
        closeAllPipes();
        return setHttpResponse(504, "Gateway Timeout", response, serv);
    }

    char buffer[1024];
    ssize_t bytesRead = read(stdout_pipe[0], buffer, 1024);
    if (bytesRead > 0)
        _response.write(buffer, bytesRead);

    int status;
    int ret = waitpid(_pid, &status, WNOHANG);
    if (ret < 0) {
        std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [500] [Internal Server Error] [Failed to wait for CGI script]";
        webServLog(logMessage, ERROR);
        closeAllPipes();
        return setHttpResponse(500, "Internal Server Error", response, serv);
    }
    else if (ret == _pid) {
        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) == 0) {
                processSuccessfulResponse(serv, response, request);
                closeAllPipes();
            } else {
                std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [500] [Internal Server Error] [CGI script exited with non-zero status]";
                webServLog(logMessage, ERROR);
                closeAllPipes();
                return setHttpResponse(500, "Internal Server Error", response, serv);
            }
        } else {
            std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [500] [Internal Server Error] [CGI script exited abnormally]";
            webServLog(logMessage, ERROR);
            closeAllPipes();
            return setHttpResponse(500, "Internal Server Error", response, serv);
        }
    }
    // else, CGI script is still running
}

// Helper method to process successful CGI response
void CGI::processSuccessfulResponse(server *serv, Response &response, Request &request)
{
    std::map<std::string, std::string> headers = extractHeaders(_response.str());
    std::string responseBody = extractBody(_response.str());
    
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
    response.setProgress(CREATE_HEADERS_STREAM);
    
    std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [200] [OK] [CGI executed]";
    webServLog(logMessage, INFO);
}
