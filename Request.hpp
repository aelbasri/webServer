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
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>

#define BUFF_SIZE 1024

enum state
{
    REQUEST_LINE,
    HEADER,
    BODY,
    DONE
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
    public:
        std::map<std::string, std::string> header;
        int parseRequestLine(int socket, int &offset, int &nBytes);
        int parseHeader(int socket, int &offset, int &nBytes);
        // void parseBody(char *buff, int &offset);
};
