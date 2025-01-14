#include "server.hpp"
#include "Request.hpp"
#include <stdlib.h>

#define BUFF_SIZE 1024

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
        char buff[BUFF_SIZE];
        int nBytes = recv(new_fd, buff, BUFF_SIZE - 1, 0);
        if (nBytes == -1)
        {
            perror("recv() failed");
            exit(1);
        }
        buff[nBytes] = '\0';
        std::cout << "===> bytes received: " << nBytes << std::endl;
        
        // write(1, buff, nBytes);
        /*Parse Request*/

        Request request;
        int offset = 0;
        enum state myState = REQUEST_LINE;

        while(myState != DONE)
        {
            switch (myState)
            {
                case  REQUEST_LINE :
                    request.parseRequestLine(buff, offset, nBytes);
                    myState = HEADER;
                    break;
                case HEADER :
                    request.parseHeader(buff, offset, nBytes);
                    myState = BODY;
                    break;
                case BODY :
                    // request.parseBody(buff, offset);
                    myState = DONE;
                    break;
                default:
                    break;
            }
        }
        std::cout << "=====> NOT BODY" << std::endl;
        write(1, buff, (offset + 1));
        std::cout << "=======> BODY" << std::endl;
        write(1, buff + (offset + 1), nBytes - (offset + 1));
        std::cout << "================" << std::endl;

        // if (header fihom content length)   
        {
            char basri[BUFF_SIZE];
            int count = nBytes - (offset + 1);
            long contentLen = strtol(request.header["Content-Length"].c_str(), NULL, 10);
            std::ofstream file("out.png", std::ios::binary);
            file.write(buff + offset + 1, count);

            int qraya = 0;
            while (count < contentLen)
            {
                //std::cout << "nbytes: " << nBytes << " and count: " << count <<  std::endl;
                if((nBytes = recv(new_fd, basri, BUFF_SIZE, 0)) <= 0)
                    break;
                file.write(basri, nBytes);
                count += nBytes;
                std::cout << ++qraya << std::endl;
            }
            file.close();
        }
        // else
        {
            
        }

        
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
        close(new_fd);
    }
}
