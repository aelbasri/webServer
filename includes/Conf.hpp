#pragma once

#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <string>
#include <vector>
#include <map>

#define nullptr  0
#define SOCKET_TIMOUT 10

class error_pages;
class config_file;
class location;
class server;
class Connection;


#include "location.hpp"
#include "server_data.hpp"
#include "error_pages.hpp"
// #include "Connection.hpp"


class Config
{
private:
    size_t     _nembre_of_server;
    std::string _fileContent;
    server *_server;
public:
    Config(std::string file);
    Config();
    ~Config();

    int loidingFile(std::string file);
    void loadContentServer();
    int CheckNumberOfServer();
    void creatPoll();

    void creatPoll2();
    void pollLoop(int ep, std::map<int, Connection*>& connections);
    int handleErrorEvent(int ep, int fd, std::map<int, Connection*>& connections);
    int handleWriteEvent(int ep, int fd, std::map<int, Connection*>& connections);
    int handleReadEvent(int ep, int fd, std::map<int, Connection*>& connections);
    void handleNewConnection(int ep, int server_fd, server* tmp, std::map<int, Connection*>& connections);
    void addSocketsToEpoll(int ep);
    int createEpollInstance();
    
    server *getServer() const;
    int get_nembre_of_server() const;
    
    std::string& setFileContent();
    int SetupServers();
};

void err();
bool isValidEroorValue(const std::string& value);
std::string trim(const std::string& input);
std::vector<std::string> StringToLines(const std::string& inputString);
bool parseBodySize(const std::string& sizeStr, long long& bytes);
bool isValidPort(const std::string& port);
bool isValidHost(const std::string& host);
std::string removeWhitespace(const std::string input);
std::string escapeSpaces(const std::string input);
void CheckKey(const std::string& key);
bool hasSpace(const std::string& str);
bool startsWithSlash(const std::string& str);
std::string intToString(int num);
void deleteTimedoutSockets(std::map<int, Connection* > &connections, int ep);
