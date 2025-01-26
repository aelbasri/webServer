#include "server.hpp"
#include "Request.hpp"
#include <stdlib.h>


Server::Server(){}



void handle_request(int new_fd)
{
    Request request;
        int offset = 0;
        int nBytes = 0;
        enum state myState = REQUEST_LINE;

        while(myState != DONE)
        {
            switch (myState)
            {
                case  REQUEST_LINE :
                    request.parseRequestLine(new_fd, offset, nBytes);
                    myState = HEADER;
                    break;
                case HEADER :
                    request.parseHeader(new_fd, offset, nBytes);
                    myState = BODY;
                    break;
                case BODY :
                    request.parseBody(new_fd, offset, nBytes);
                    myState = DONE;
                    break;
                default:
                    break;
            }
        }
        /*std::cout << "nik smoooook" << std::endl;*/

        std::string s;
        std::string fille_content;
        std::ifstream f("page_error.html");

        if (!f.is_open()) {
           exit(1);
        }


        while (getline(f, s)){
            fille_content += s;
            fille_content.push_back('\n');
        }
        f.close();

        // std::cout  << fille_content << std::endl; 
        const std::string html_content = fille_content;
        // "<!DOCTYPE html>\n"
        // "<html lang=\"en\">\n"
        // "<head>\n"
        // "    <meta charset=\"UTF-8\">\n"
        // "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        // "    <title>Hello World</title>\n"
        // "    <style>\n"
        // "        body {\n"
        // "            font-family: Arial, sans-serif;\n"
        // "            display: flex;\n"
        // "            justify-content: center;\n"
        // "            align-items: center;\n"
        // "            height: 100vh;\n"
        // "            margin: 0;\n"
        // "            background-color: #f0f0f0;\n"
        // "        }\n"
        // "        h1 {\n"
        // "            color: #333;\n"
        // "            padding: 20px;\n"
        // "            border-radius: 5px;\n"
        // "            background-color: white;\n"
        // "            box-shadow: 0 2px 4px rgba(0,0,0,0.1);\n"
        // "        }\n"
        // "    </style>\n"
        // "</head>\n"
        // "<body>\n"
        // "    <h1>Hello, World!</h1>\n"
        // "</body>\n"
        // "</html>\n";

        std::ostringstream ss;
        ss << "HTTP/1.1 200 OK\r\n"
        << "Content-Type: text/html; charset=UTF-8\r\n"
        << "Content-Length: " << fille_content.length() << "\r\n"
        << "Connection: close\r\n"
        << "\r\n"
        << fille_content;

        std::string response = ss.str();

        /*std::cout << "Got: (" << buff << ")" << std::endl;*/
        /*if (send(new_fd, "Hello Webserv!", strlen("Hello Webserv!"), 0) == -1)*/
        if (send(new_fd, response.c_str(), response.size(), 0) == -1)
        {
            perror("send() failed");
            exit(1);
        }

}


int Server::run()
{


    int yes = 1;
    int addI;

    std::map<std::string, std::string> mymap;
    std::map<std::string, std::string>::iterator it;
    config_file Conf("config_file.yaml"); 

    Conf.loadContentServer();
    _data = Conf;
    server *_server = _data.getServer();
    int NbServer = _data.get_nembre_of_server();
    cout << "nembre of server is : " << NbServer << endl;



    for (int i = 0; i < NbServer ; i++)
    {
        cout << "-------- server n "<< i << "-----------" << endl;
        cout << "name : " << _server[i].Get_name() << endl; 
        cout << "host : " << _server[i].Get_host() << endl; 
        cout << "port : " << _server[i].Get_port() << endl; 
        cout << "max budy : " << _server[i].Get_max_body_size() << endl; 

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


void Server::printRequest() const
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
