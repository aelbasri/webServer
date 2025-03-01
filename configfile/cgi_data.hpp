#pragma once

#include "server_data.hpp"
#include <fstream>
#include <iostream>
#include <cstdio>
#include <ctime>
#include <sys/wait.h>
#include <sys/time.h>
#include <sstream>


class CGI
{
private:
    std::string _PathOfExecutable;
    std::string _type;
    std::string _path;
    int _ExitStatus;

public:
    CGI(std::string __path);
    CGI();
    ~CGI();

    void SetPath(std::string __type);
    void SetType(std::string __indix);

    std::string GetPath() const;
    std::string GetType() const;
    int GetExitStatus() const;

    std::string RunCgi(const std::string& requestBody);
};

