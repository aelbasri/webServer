
#pragma once

#include <fstream>
#include <iostream>
#include <cstdio>
#include <ctime>
#include <sys/time.h>
#include <sstream>

class CGI
{
private:
    std::string _PathOfExecutable;
    std::string _type;
    std::string _path;
public:

    void RunPythonCgi();
    CGI();
    ~CGI();
};

