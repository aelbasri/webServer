#include "CGI.hpp"

CGI::CGI(){}

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
        command << "python3 " << _path;
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

