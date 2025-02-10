

#include "cgi_data.hpp"

CGI::CGI(){}

CGI::~CGI()
{

}

void CGI::SetPath(std::string __path){
    _path = __path;
}


void CGI::SetType(std::string __type){
    _type = __type;
}


std::string CGI::GetPath(){
    return _path;
}


std::string CGI::GetType(){
    return _type;
}


std::string generate_unique_path() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    
    std::stringstream ss;
    ss << "/tmp/." << tv.tv_sec << tv.tv_usec << ".html";
    return ss.str();
}

std::string  CGI::RunCgi() {    
        std::stringstream  command ;
        command << _type.c_str() << _path;
        std::string tmp = command.str();

    FILE* pipe = popen(tmp.c_str(), "r");
    if (!pipe) {
        std::cerr << "Error executing command." << std::endl;
        return NULL;
    }

    char buffer[128];
    std::string result = "";
    while (fgets(buffer, sizeof(buffer), pipe) != 0) {
        result += buffer;
    }

    pclose(pipe);
    return(result);
}