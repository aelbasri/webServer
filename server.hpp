#pragma once

#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <sys/epoll.h>
#include "Connection.hpp"


class Server
{
    private:
        std::vector<Connection> connections;
        std::string _host;
        std::string _port;
        int         _sock;

    public:
        Server();
        Server(std::string host, std::string port);
        /*~Server();*/

        int run();
        void creatPoll() const;
    public:
        class InternalServerError : public std::exception
        {
            public:
                const char* what() const throw(); 
        };
};
