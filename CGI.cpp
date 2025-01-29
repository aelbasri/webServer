#include "CGI.hpp"

CGI::CGI(){
    std::map<std::string, std::string> _mapCgi = _server->GetCgi();
    for (std::map<std::string, std::string>::iterator i = _mapCgi.begin(); i != _mapCgi.end(); i++)
    {
        if (i->first == "type")
            _type = i->second;
        else
            _path = i->second;
    }
}

CGI::~CGI(){}

std::string generate_unique_path() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    
    std::stringstream ss;
    ss << "/tmp/." << tv.tv_sec << tv.tv_usec << ".html";
    return ss.str();
}

void   CGI::RunPythonCgi() {
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

