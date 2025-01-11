#pragma once

#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>
#include <cstdio>
#include <map>

#define REQUEST_LINE 1
#define HEADER 2
#define BODY 3

class Request
{
    private:
        //request line
        std::string method;
        std::string requestTarget;
        std::string httpVersion;

        //header
        std::map<std::string, std::string> header;
    public:
        int parseRequestLine(char *buff, int &offset, int nBytes);
        int parseHeader(char *buff, int &offset, int nBytes);
        // void parseBody(char *buff, int &offset);
};