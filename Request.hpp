#pragma once

#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>
#include <cstdio>

class Request
{
    private:
        std::string method;
        std::string requestTarget;
        std::string httpVersion;
    public:
        int parseRequestLine(std::stringstream &bufferStream);
        void parseHeader();
        void parseBody();
};