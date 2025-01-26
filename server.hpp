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

#include "configfile/config_file.hpp"


class Server
{
    private:
        config_file _data;


    public:
        Server();
        // Server();
        /*~Server();*/
    void test();
        int run();
        void printRequest() const;
};

