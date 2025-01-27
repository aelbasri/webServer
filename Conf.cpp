
#include "Conf.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "configfile/server_data.hpp"
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

void Config::creatPoll()
{
    // struct epoll_event ev;
    struct epoll_event evlist[MAX_EVENT];

    int ep = epoll_create(1);
    if (ep == -1)
    {
        // Throw exception
        std::cout << "epoll" << std::endl;
        return;
    }

    for(size_t i = 0; i < _nembre_of_server; i++)
    {
        struct epoll_event ev;
        ev.data.fd = _server[i].getSock();
        ev.events = EPOLLIN;
        if (epoll_ctl(ep, EPOLL_CTL_ADD, _server[i].getSock(), &ev) == -1)
        {
            // Throw exception )
            return;
        }
    }

    while(1)
    {
        int nbrReady = epoll_wait(ep, evlist, MAX_EVENT, -1);
        if(nbrReady < 0)
        {
            // Throw exception
            return;
        }
        for(int i = 0; i < nbrReady; i++)
        {
            if(evlist[i].events & EPOLLIN)
            {
                int server_fd = -1;
                for(size_t j = 0; j < _nembre_of_server; j++)
                {
                    if (evlist[i].data.fd == _server[j].getSock())
                    {
                        server_fd = _server[j].getSock();
                        break;
                    }
                }
                if (server_fd != -1)
                {
                    int new_fd = accept(server_fd, NULL,  0);
                    evlist[i].data.fd = new_fd;
                    evlist[i].events = EPOLLIN;
                    if (epoll_ctl(ep, EPOLL_CTL_ADD, new_fd, &evlist[i]) == -1)
                    {
                        // Throw exception
                        return;
                    }
                }
                else
                {
                    handle_request(evlist[i].data.fd);
                    close(evlist[i].data.fd);
                }
            }
        }
    }
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
    }
    creatPoll();
    return (0);
}


