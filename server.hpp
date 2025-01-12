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

class Server
{
    private:
        std::string _host;
        std::string _port;
        int         _sock;

    public:
        Server();
        Server(std::string host, std::string port);
        /*~Server();*/

        int run();
        void printRequest() const;
};
