

#include "cgi_data.hpp"

CGI::CGI(){}

CGI::CGI(std::string __path): _path(__path){} 

CGI::~CGI(){}

void CGI::SetPath(std::string __path){
	_path = __path;
}

void CGI::SetType(std::string __type){
	_type = __type;
}

std::string CGI::GetPath() const{
	return (_path);
}

std::string CGI::GetType() const{
	return (_type);
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
            std::cout << "HEADER: " << key << " = " << value << std::endl;
        }
    }
    return headers;
}
std::string extractBody(const std::string& output) {
    // size_t pos = output.find("\r\n\r\n");
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
        queryString = queryString.substr(0, queryString.length() - 1); // Remove the trailing '&'
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
    
    std::cout << "PATH_INFO: " << pathInfo << std::endl;
    std::cout << "PATH: " << scriptPath << std::endl;
    return pathInfo;
}

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
            return ;
        }
    }


    std::string scriptPath = ".";
    // scriptPath += CGI_PATH;
    std::string pathInfo = ScriptPath_PathInfo(scriptPath, request.getRequestTarget());

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
    std::cout << "INTER: " << interpreter << std::endl;
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


    const std::map<std::string, std::string>& headers = request.getHeaders();
    std::map<std::string, std::string>::const_iterator it;
    for (it = headers.begin(); it != headers.end(); ++it) {
        std::string envVar = "HTTP_" + it->first;
        // Replace dashes with underscores
        for (std::string::iterator charIt = envVar.begin(); charIt != envVar.end(); ++charIt) {
            if (*charIt == '-') {
                *charIt = '_';
            }
        }
        // Convert to uppercase
        for (std::string::iterator charIt = envVar.begin(); charIt != envVar.end(); ++charIt) {
            if (*charIt >= 'a' && *charIt <= 'z') {
                *charIt = *charIt - 32;
            }
        }
        env[envVar] = it->second;
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

        close(stdin_pipe[1]);
        close(stdout_pipe[0]);

        dup2(stdin_pipe[0], STDIN_FILENO);
        close(stdin_pipe[0]);

        dup2(stdout_pipe[1], STDOUT_FILENO);
        close(stdout_pipe[1]);

        std::map<std::string, std::string>::const_iterator it;
        for (it = env.begin(); it != env.end(); ++it) {
            setenv(it->first.c_str(), it->second.c_str(), 1);
        }


        std::vector<const char*> args;
        
        if (!interpreter.empty()) {
            args.push_back(interpreter.c_str());
        }
        args.push_back(scriptPath.c_str());
        args.push_back(NULL);



        execv(args[0], const_cast<char* const*>(&args[0]));
        std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [500] [Internal Server Error] [Unsupported script type]";
        webServLog(logMessage, ERROR);
        response.setProgress(BUILD_RESPONSE);
        throw server::InternalServerError();
       } else {
 
        close(stdin_pipe[0]);
        close(stdin_pipe[1]);
        close(stdout_pipe[1]);

        std::ostringstream output;
        char buffer[1024];
        ssize_t bytesRead;
        while ((bytesRead = read(stdout_pipe[0], buffer, sizeof(buffer))) > 0) {
            output.write(buffer, bytesRead);
        }
        close(stdout_pipe[0]);
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) == 0 || WEXITSTATUS(status) == 1)
            {
                // extract headers and body from the output
                std::map<std::string, std::string> headers = extractHeaders(output.str());
                std::string responseBody = extractBody(output.str());
                std::map<std::string, std::string>::const_iterator it;
                for (it = headers.begin(); it != headers.end(); ++it) {
                    response.addHeader(it->first, it->second);
                    if (it->first == "Set-Cookie")
                    {
                        // std::cout << 
                        std::string cookie = it->second.substr(strlen("session_id= "));
                        std::cout << "COOOOKKKKKIE: " << cookie << std::endl;
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
                return ;
            }
        }
        std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [500] [Internal Server Error] [CGI script failed]";
        webServLog(logMessage, ERROR);
        response.setProgress(BUILD_RESPONSE);
        throw server::InternalServerError();
    } 
    return ;
}
