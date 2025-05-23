#pragma once

// #include "server_data.hpp"
#include <fstream>
#include <iostream>
#include <cstdio>
#include <ctime>
#include <sys/wait.h>
#include <sys/time.h>
#include <sstream>
#include "Response.hpp"
#include "log.hpp"


#define CGI_PATH "/cgi-bin/"
#define CGI_TIMEMOUT_SECONDS 5

class server;

class Response;

enum CGIStatus
{
    CGI_SETUP,
    CGI_BODY_READING,
    CGI_FORKING,
    CGI_RUNNING,
    CGI_DONE,
};

class CGI
{
    private:
        std::string scriptPath;
        std::string pathInfo;
        std::string interpreter;

        int stdin_pipe[2];
        int stdout_pipe[2];
        time_t _start_time;
        CGIStatus _status;
        pid_t _pid;
        std::ostringstream _response;


    public:
        CGI();
        ~CGI();

        void setScriptPath(std::string __scriptPath);
        void setPathInfo(std::string __pathInfo);
        bool pipesNotSet() const;
        enum CGIStatus getCGIstatus() const;

        void RunCgi(server *serv, Response &response, Request &request);
        void setupCGI(server *serv, Response &response, Request &request);
        void processSuccessfulResponse(server *serv, Response &response, Request &request);
        void closeAllPipes();
        bool processTimedOut() const;
        void forkChild(server *serv, Response &response, Request &request);
};

std::string ScriptPath_PathInfo(std::string& scriptPath, const std::string& requestTarget);
std::string getInterpreter(const std::string& filePath);
std::string convertQueryMapToString(const std::map<std::string, std::string>& query);
std::map<std::string, std::string> extractHeaders(const std::string& output);
std::string extractBody(const std::string& output);

