
#include "Conf.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include <stdlib.h>

Config::Config():_server(nullptr){}

Config::~Config(){
    if (_server){
        delete[] _server;
        _server = NULL;
    }
}

Config::Config(std::string file): _server(nullptr){
    loidingFile(file);
    _nembre_of_server = CheckNumberOfServer();
    _server =  new server[_nembre_of_server];
    
}

void err(){
    std::cerr << "error of open" << std::endl;
    exit(1);
}

server *Config::getServer(){
    return(_server);
}

void Config::loadContentServer() {
    std::string sentence = "server";
    size_t pos = 0;
    
    for (size_t i = 0; i < _nembre_of_server; i++) {
        pos = _fileContent.find(sentence, pos);
        if (pos == std::string::npos)
            break;
        pos += sentence.length() + 1;
        size_t nextServerPos = _fileContent.find(sentence, pos);        
        std::string serverContent;
        if (nextServerPos != std::string::npos)
            serverContent = _fileContent.substr(pos, nextServerPos - pos);
        else 
            serverContent = _fileContent.substr(pos);
        
        _server[i].Set_content(serverContent);        
        if (nextServerPos != std::string::npos)
            pos = nextServerPos;
        _server[i].Set_nembre_of_location(_server[i].CheckNumberOfLocation());
        _server[i].new_location();
        _server[i].loadingDataserver();
    }
}

void Config::loidingFile(std::string file){
    std::string s;
    std::string strCRLF = "\r\n";

    std::ifstream f(file.c_str());

    if (!f.is_open()) {
        err();
    }
    while (getline(f, s)){

        setFileContent() += s;
        if (!s.empty())
            setFileContent().push_back('\n');
    }
    f.close();
}

int Config::get_nembre_of_server(){
    return(_nembre_of_server);
}

std::string& Config::setFileContent(){
    return(_fileContent);
}

int Config::CheckNumberOfServer(){
    std::string sentence = "server:";
    int words = 0;
    size_t pos = 0;

    std::string tmp;

    tmp = escapeSpaces(_fileContent);

    while (((pos = tmp.find(sentence, pos)) != (size_t)std::string::npos) && !sentence.empty()) {
            pos += sentence.length();
            words++;
    }
    return words;
}

int Config::SetupServers()
{
    std::cout << " _nembre_of_server : ============"<< _nembre_of_server << std::endl; 
    for (size_t i = 0; i < _nembre_of_server ; i++)
    {
        std::cout << "i: ============"<< i << std::endl; 
        std::cout << getpid() << std::endl;
                
        if (_server[i].run() == -1)
            exit(1);
        _server[i].creatPoll();
    }
    return (0);
}


