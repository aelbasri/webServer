

#include "cgi_data.hpp"

CGI::CGI(){}

CGI::CGI(std::string __path, std::string __type): _type(__type), _path(__path){} 

CGI::~CGI(){}

void CGI::SetPath(std::string __path){
	_path = __path;
}

void CGI::SetType(std::string __type){
	_type = __type;
}

std::string CGI::GetPath(){
	return (_path);
}

std::string CGI::GetType(){
	return (_type);
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

#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

std::string CGI::RunCgi(const std::string &requestBody) {
    if (!isFileValid(_path)) return "";

    int in_pipe[2], out_pipe[2];
    if (pipe(in_pipe) < 0 || pipe(out_pipe) < 0) return "";

    pid_t pid = fork();
    if (pid < 0) return "";

    if (pid == 0) { // Child
        close(in_pipe[1]);
        close(out_pipe[0]);
        
        dup2(in_pipe[0], STDIN_FILENO);
        dup2(out_pipe[1], STDOUT_FILENO);
        
        close(in_pipe[0]);
        close(out_pipe[1]);

        execl(_path.c_str(), _path.c_str(), NULL);
        exit(1); // If exec fails
    }
    else { // Parent
        close(in_pipe[0]);
        close(out_pipe[1]);

        // Write request body
        write(in_pipe[1], requestBody.c_str(), requestBody.size());
        close(in_pipe[1]);

        // Read response (single read operation)
        char buffer[4096];
        int bytes = read(out_pipe[0], buffer, sizeof(buffer));
        close(out_pipe[0]);

        waitpid(pid, NULL, 0);
        return bytes > 0 ? std::string(buffer, bytes) : "";
    }
}
