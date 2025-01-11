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
        std::cout << "===> bytes received: " << nBytes << std::endl;
        if (nBytes == -1)
        {
            perror("recv() failed");
            exit(1);
        }
        buff[nBytes] = '\0';
        std::cout << "===> lWLA" << std::endl;
        write(1, buff, nBytes);
        std::cout << "===> lWLA SALAT" << std::endl;
        /*Parse Request*/

        Request request;
        int offset = 0;
        int state = REQUEST_LINE;

        while(state != 0)
        {
            switch (state)
            {
                case  REQUEST_LINE :
                    request.parseRequestLine(buff, offset, nBytes);
                    state = HEADER;
                    break;
                case HEADER :
                    request.parseHeader(buff, offset, nBytes);
                    state = BODY;
                    break;
                case BODY :
                    // request.parseBody(buff, offset);
                    state = 0;
                    break;
                default:
                    break;
            }
        }

        {
            std::cout << "=============================>" << std::endl;
            char basri[BUFF_SIZE];
            int count = 0;
            int qraya = 0;
            while ((nBytes = recv(new_fd, basri, BUFF_SIZE, 0)) != -1)
            {
                count += nBytes;
                std::cout << "nbytes: " << nBytes << " and count: " << count <<  std::endl;
                /*if (count >= strtod(request.header["Content-Length"], NULL) - 100)*/
                if (count >= 20000 || nBytes == 0)
                    break;
                write(1, basri, nBytes);
                qraya++;
            }
            std::cout << "=============================> END " << qraya << std::endl;
        }

        //check that ascii
        // std::stringstream bufferStream(buff);
        // request.parseRequestLine(bufferStream);
        // request.parseHeader(bufferStream);   
        //

        //==============
        /*std::cout << "Got: (" << buff << ")" << std::endl;*/
        if (send(new_fd, "Hello Webserv!", strlen("Hello Webserv!"), 0) == -1)
        {
            perror("send() failed");
            exit(1);
        }
        close(new_fd);
        std::cout << "khrj==================" << std::endl;
    }
}
