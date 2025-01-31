

#include "cgi_data.hpp"

CGI::CGI(){}


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

void   CGI::RunCgi() {
    std::string filename = generate_unique_path();
    
        std::stringstream  command ;
        command << _type.c_str() << _path;
        std::string tmp = command.str();

    FILE* pipe = popen(tmp.c_str(), "r");
    if (!pipe) {
        std::cerr << "Error executing command." << std::endl;
        return;
    }

    char buffer[128];
    std::string result = "";
    while (fgets(buffer, sizeof(buffer), pipe) != 0) {
        result += buffer;
    }

    pclose(pipe);


    std::ofstream file;
    file.open(filename.c_str());

    if (!file.is_open())
    {
        std::cout << "Error in creating file!" << std::endl;
        return;
    }
    file << "<html><body><h1>Script Output</h1><pre>" << result << "</pre></body></html>";

    _PathOfExecutable = filename;
    return ;
}