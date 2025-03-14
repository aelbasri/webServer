
#include "Conf.hpp"
#include "Connection.hpp"
#include "server_data.hpp"
#include <stdlib.h>
#include "colors.hpp"
#include "log.hpp"
#include <exception>


std::string intToString(int num) {
    std::ostringstream oss;
    oss << num;
    return oss.str();
}

Config::Config():_server(nullptr){}

Config::~Config(){
    if (_server){
        delete[] _server;
        _server = NULL;
    }
}

Config::Config(std::string file): _server(nullptr){
    if (loidingFile(file))
        throw std::runtime_error("failed to load config file!");
    _nembre_of_server = CheckNumberOfServer();
    if (_nembre_of_server < 1)
        throw std::runtime_error("invalid syntax in configfile");
    _server =  new server[_nembre_of_server];
    
}

void err(){
    std::cerr << "error of open" << std::endl;
}

server *Config::getServer() const{
    return(_server);
}

void Config::loadContentServer() {
    std::string sentence = "server";
    size_t pos = 0;
    
    for (size_t i = 0; i < _nembre_of_server; i++) {
        pos = _fileContent.find(sentence, pos);
        if (pos == std::string::npos)
            break;
        pos += sentence.length() + 1;
        size_t nextServerPos = _fileContent.find(sentence, pos);        
        std::string serverContent;
        if (nextServerPos != std::string::npos)
            serverContent = _fileContent.substr(pos, nextServerPos - pos);
        else 
            serverContent = _fileContent.substr(pos);
        
        _server[i].Set_content(serverContent);        
        if (nextServerPos != std::string::npos)
            pos = nextServerPos;
        _server[i].Set_number_of_location(_server[i].CheckNumberOfLocation());
        _server[i].new_location();
        _server[i].loadingDataserver();
    }
}

int Config::loidingFile(std::string file){
    std::string s;
    std::string strCRLF = "\r\n";

    std::ifstream f(file.c_str());

    if (!f.is_open()) {
        err();
        return -1;
    }
    while (getline(f, s)){
        if (s.find("#") != std::string::npos) continue;
        setFileContent() += s;

        if (!s.empty())
            setFileContent().push_back('\n');
    }
    f.close();
    return 0;
}

int Config::get_nembre_of_server() const{
    return(_nembre_of_server);
}

std::string& Config::setFileContent(){
    return(_fileContent);
}

int Config::CheckNumberOfServer(){
    std::string sentence = "server:";
    int words = 0;
    size_t pos = 0;

    std::string tmp;

    tmp = escapeSpaces(_fileContent);

    while (((pos = tmp.find(sentence, pos)) != (size_t)std::string::npos) && !sentence.empty()) {
            pos += sentence.length();
            words++;
    }
    return words;
}

void deleteTimedoutSockets(std::map<int, Connection* > &connections, int ep)
 {
     std::vector<Connection*> toDelete;
     time_t current_time = time(NULL);
        for(std::map<int, Connection *>::iterator it = connections.begin(); it != connections.end(); it++)
        {
            int fd = it->first;
            if (current_time - it->second->getStartTime() > SOCKET_TIMOUT)
            {
                toDelete.push_back(it->second);
                close(fd);
                epoll_ctl(ep, EPOLL_CTL_DEL, fd, NULL);
                std::string logMessage = "[CONNECTION CLOSED] [SOCKET_FD: " + intToString(fd) + " AFTER TIMEOUT]";
                webServLog(logMessage, INFO);
            }
        }
        for (size_t i = 0; i < toDelete.size(); i++)
        {
            int _fd = toDelete[i]->getSocket();
            delete toDelete[i];
            connections.erase(_fd);
        }
 }

// Function to recursively remove directory contents
void removeDirectoryContents(const std::string& path) {
    DIR* dir = opendir(path.c_str());
    if (!dir) {
        std::cerr << "Failed to open directory: " << path << " - " << strerror(errno) << std::endl;
        throw std::runtime_error("Failed to open directory");
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        // Skip "." and ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        std::string filePath = path + "/" + entry->d_name;

        // Check if it's a directory
        struct stat info;
        if (stat(filePath.c_str(), &info) == 0) {
            if (S_ISDIR(info.st_mode)) {
                // Recursively remove subdirectories
                removeDirectoryContents(filePath);
                rmdir(filePath.c_str());
            } else {
                // Remove files
                std::remove(filePath.c_str());
            }
        }
    }
    closedir(dir);
}

// Function to remove and recreate the directory
void removeAndRecreateDirectory(const std::string& path) {
    // Check if the directory exists
    struct stat info;
    if (stat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode)) {
        // Remove directory contents
        removeDirectoryContents(path);

        // Remove the directory itself
        if (rmdir(path.c_str()) != 0) {
            std::cerr << "Failed to remove directory: " << path << " - " << strerror(errno) << std::endl;
            throw std::runtime_error("Failed to open directory");
        }
    }

    // Create the directory
    if (mkdir(path.c_str(), 0777) != 0) {
        std::cerr << "Failed to create directory: " << path << " - " << strerror(errno) << std::endl;
        throw std::runtime_error("Failed to open directory");
    }
}

int Config::SetupServers()
{
    for (size_t i = 0; i < _nembre_of_server ; i++)
    {
        if (_server[i].run() == -1)
            throw std::runtime_error("Failed to open directory");
    }
    removeAndRecreateDirectory(UPLOAD_DIRECTORY);
    creatPoll();
    return (0);
}


