

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

std::string CGI::RunCgi(const std::string &requestBody) {
    if (!isFileValid(_path)) {
        std::cout << "File '" << _path << "' is invalid/missing" << std::endl;
        return "";
    }

    int stdin_pipe[2], stdout_pipe[2];
    if (pipe(stdin_pipe) || pipe(stdout_pipe)) {
        perror("pipe failed");
        return "";
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        return "";
    }

    if (pid == 0) { 
        dup2(stdin_pipe[0], STDIN_FILENO); 
        dup2(stdout_pipe[1], STDOUT_FILENO);

        close(stdin_pipe[0]); close(stdin_pipe[1]);
        close(stdout_pipe[0]); close(stdout_pipe[1]);

        std::string interpreter = getInterpreter(_path);
        std::vector<const char*> args;
        
        if (!interpreter.empty()) {
            args.push_back(interpreter.c_str());
        }
        args.push_back(_path.c_str());
        args.push_back(NULL);

        execv(args[0], const_cast<char* const*>(&args[0])); 
        perror("execv failed");
        _exit(EXIT_FAILURE); 
    }
    else {
        close(stdin_pipe[0]); close(stdout_pipe[1]);

        write(stdin_pipe[1], requestBody.c_str(), requestBody.size()); 
        close(stdin_pipe[1]);

        char buffer[4096];
        ssize_t bytes_read = read(stdout_pipe[0], buffer, sizeof(buffer));
        close(stdout_pipe[0]); 

        int status;
        waitpid(pid, &status, 0); 

        if (WIFEXITED(status)) {
            _ExitStatus = WEXITSTATUS(status);
        }
        else if (WIFSIGNALED(status)) {
            _ExitStatus = WTERMSIG(status); 
        }
        else {
            _ExitStatus = -1;
        }
        return bytes_read > 0 ? std::string(buffer, bytes_read) : "";
    }
}
