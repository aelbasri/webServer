#pragma once

#include "server_data.hpp"
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
    CGI(std::string __path, std::string __type);
    CGI();
    ~CGI();

    void SetPath(std::string __type);
    void SetType(std::string __indix);

    std::string GetPath();
    std::string GetType();

    std::string RunCgi(const std::string& requestBody);
};

