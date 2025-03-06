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

class error_pages;
class config_file;
class location;
class server;


#include "configfile/location.hpp"
#include "configfile/server_data.hpp"
#include "configfile/error_pages.hpp"


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

    void loidingFile(std::string file);
    void loadContentServer();
    int CheckNumberOfServer();
    void creatPoll();

    
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
