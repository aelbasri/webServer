#include "server.hpp"
#include "Request.hpp"
#include <stdlib.h>

servers::servers():_server(nullptr){}

servers::~servers(){
    if (_server){
        delete[] _server;
        _server = NULL;
    }
}

servers::servers(std::string file): _server(nullptr){
    loidingFile(file);
    _nembre_of_server = CheckNumberOfServer();
    _server =  new server[_nembre_of_server];
    
}

void err(){
    std::cerr << "error of open" << std::endl;
    exit(1);
}

server *servers::getServer(){
    return(_server);
}

void servers::loadContentServer() {
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

void servers::loidingFile(std::string file){
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

int servers::get_nembre_of_server(){
    return(_nembre_of_server);
}

std::string& servers::setFileContent(){
    return(_fileContent);
}

int servers::CheckNumberOfServer(){
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



int servers::run()
{


    int yes = 1;
    int addI;

    std::map<std::string, std::string> mymap;
    std::map<std::string, std::string>::iterator it;


    server *_server = getServer();
    int NbServer = get_nembre_of_server();
    std::cout << "nembre of server is : " << NbServer << std::endl;



    for (int i = 0; i < NbServer ; i++)
    {
        std::cout << "-------- server n "<< i << "-----------" << std::endl;
        std::cout << "name : " << _server[i].Get_name() << std::endl; 
        std::cout << "host : " << _server[i].Get_host() << std::endl; 
        std::cout << "port : " << _server[i].Get_port() << std::endl; 
        std::cout << "max budy : " << _server[i].Get_max_body_size() << std::endl; 
        std::cout << "-------error_pages"<< i <<" ---------" << std::endl;
        mymap = _server[i].GetErr();
        for (it=mymap.begin(); it!=mymap.end(); ++it)
          std::cout << it->first << " => " << it->second << '\n';
    //    std::cout <<  "_default  : " <<_server[i].GetDefault() << std::endl;




        int _sock = _server[i].getSock();
        struct addrinfo hints = _server[i].getHints();
        struct addrinfo *res = _server[i].getRes();
        struct addrinfo *p = _server[i].getP();

        memset(&hints, 0,  sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        if ((addI = getaddrinfo(NULL, _server[i].Get_port().c_str(), &hints, &res)) != 0)
        {
            std::cerr << gai_strerror(addI) << std::endl;
            return -1;
        }

        for(p = res; p; p = p->ai_next)
        {
            if ((_sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
                continue;
            if ((setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) == -1)
            {
                perror("setsockopt");
                return (-1);
                /*exit(1);*/
            }
            //bind
            if (bind(_sock, res->ai_addr, res->ai_addrlen) == -1)
                continue;
            break;
        }

        freeaddrinfo(res); 
        if (!p)
        {
            perror("bind failed");
            return (-1);
            /*exit(1);*/
        }

        if (listen(_sock, 10) == -1)
        {
            perror("listen() failed");
            return (-1);
            /*exit(1);*/
        }

    }
    
    // std::cout << "Server is listening on " << _host << ":" << _port << std::endl; 
    return (0);
}


void servers::printRequest() const
{
    while(1)
    {
        
        // int new_fd = accept(_sock,NULL, 0);
        // std::cout << "===> connection accepted" << std::endl;


        
        // if (new_fd == -1)
        // {
        //     perror("accept() failed");
        //     exit(1);
        // }
        
        // /*Parse Request*/
        // handle_request(new_fd);
        // close(new_fd);
    }
}