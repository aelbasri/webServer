#include "server.hpp"
#include "Request.hpp"
#include <stdlib.h>


Server::Server() : _host("127.0.0.1"), _port("3000") {}

Server::Server(std::string host, std::string port) : _host(host), _port(port) {}

int Server::run()
{
    struct addrinfo hints;
    struct addrinfo *res ,*p;
    int yes = 1;
    int addI;

    memset(&hints, 0,  sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if ((addI = getaddrinfo(NULL, _port.c_str(), &hints, &res)) != 0)
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

    std::cout << "Server is listening on " << _host << ":" << _port << std::endl; 
    return (0);
}

void Server::printRequest() const
{
    while(1)
    {
        int new_fd = accept(_sock, NULL, 0);
        std::cout << "===> connection accepted" << std::endl;
        
        if (new_fd == -1)
        {
            perror("accept() failed");
            exit(1);
        }
        
        /*Parse Request*/

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
        const std::string html_content = 
        "<!DOCTYPE html>\n"
        "<html lang=\"en\">\n"
        "<head>\n"
        "    <meta charset=\"UTF-8\">\n"
        "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        "    <title>Hello World</title>\n"
        "    <style>\n"
        "        body {\n"
        "            font-family: Arial, sans-serif;\n"
        "            display: flex;\n"
        "            justify-content: center;\n"
        "            align-items: center;\n"
        "            height: 100vh;\n"
        "            margin: 0;\n"
        "            background-color: #f0f0f0;\n"
        "        }\n"
        "        h1 {\n"
        "            color: #333;\n"
        "            padding: 20px;\n"
        "            border-radius: 5px;\n"
        "            background-color: white;\n"
        "            box-shadow: 0 2px 4px rgba(0,0,0,0.1);\n"
        "        }\n"
        "    </style>\n"
        "</head>\n"
        "<body>\n"
        "    <h1>Hello, World!</h1>\n"
        "</body>\n"
        "</html>\n";

        std::ostringstream ss;
        ss << "HTTP/1.1 200 OK\r\n"
        << "Content-Type: text/html; charset=UTF-8\r\n"
        << "Content-Length: " << html_content.length() << "\r\n"
        << "Connection: close\r\n"
        << "\r\n"
        << html_content;

        std::string response = ss.str();

        /*std::cout << "Got: (" << buff << ")" << std::endl;*/
        /*if (send(new_fd, "Hello Webserv!", strlen("Hello Webserv!"), 0) == -1)*/
        if (send(new_fd, response.c_str(), response.size(), 0) == -1)
        {
            perror("send() failed");
            exit(1);
        }
        // file.close();
        close(new_fd);
    }
}
