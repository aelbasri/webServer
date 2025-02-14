

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

std::string CGI::RunCgi(const std::string &requestBody){
    if (!isFileValid(_path)) {
        //throw exception
        std::cout << "File '" << _path << "' is not valid or does not exist." << std::endl;
        return "";
    }

    std::string interpreter = getInterpreter(_path);
    if (!interpreter.empty()) {
        std::cout << "Interpreter for " << _path << ": " << interpreter << std::endl;
    } else {
        std::cout << "No interpreter found for " << _path << std::endl;
    }
    std::string command = interpreter + " " + _path;
    FILE *pipe;
    char buffer[128];

    std::cout << "---------------->>>>>>>> " << requestBody << std::endl;

    pipe = popen(command.c_str(), "w");
    if (!pipe)
    {
        std::cerr << "Error executing command." << std::endl;
        return "";
    }

    fwrite(requestBody.c_str(), 1, requestBody.size(), pipe);
    pclose(pipe);

    pipe = popen(command.c_str(), "r");
    if (!pipe)
    {
        std::cerr << "Error executing command." << std::endl;
        return "";
    }

    std::string result = "";
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
    {
        result += buffer;
    }
    pclose(pipe);
    return result;
}
