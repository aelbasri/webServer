
#pragma once


#include "configfile/server_data.hpp"
#include <fstream>
#include <iostream>
#include <cstdio>
#include <ctime>
#include <sys/time.h>
#include <sstream>

class CGI
{
private:
    server *_server;
    std::string _PathOfExecutable;
    std::string _type;
    std::string _path;
public:

    void RunPythonCgi();
    CGI();
    ~CGI();
};

