#pragma once

#include "server_data.hpp"
#include <fstream>
#include <iostream>
#include <cstdio>
#include <ctime>
#include <sys/wait.h>
#include <sys/time.h>
#include <sstream>
#include "../Response.hpp"
#include "../log.hpp"


#define CGI_PATH "/cgi-bin/"


class Response;

class CGI
{
private:
    std::string _path;
    int _ExitStatus;

public:
    CGI(std::string __path);
    CGI();
    ~CGI();

    void SetPath(std::string __type);
    
    std::string GetPath() const;
    int GetExitStatus() const;

    void RunCgi(server *serv, Response &response, Request &request);
};

