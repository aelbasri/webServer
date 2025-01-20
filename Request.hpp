#pragma once

#include <string>
#include <cstring>
#include <fstream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdio>
#include <map>
#include <cmath>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>


#define BUFF_SIZE 1025

enum state
{
    REQUEST_LINE,
    HEADER,
    BODY,
    DONE
};

struct parseBodyElement
{
    int offset;
    int nBytes;
    int consumed;
    char *buffer;
    std::ofstream file;
};

class Request
{
    private:
        char buffer[BUFF_SIZE];

        //request line
        std::string method;
        std::string requestTarget;
        std::string httpVersion;

        //header
        /*std::map<std::string, std::string> header;*/
        std::map<std::string, std::string> header;
    public:
        int parseRequestLine(int socket, int &offset, int &nBytes);
        int parseHeader(int socket, int &offset, int &nBytes);
        int parseBody(int socket, int &offset, int &nBytes);
};
