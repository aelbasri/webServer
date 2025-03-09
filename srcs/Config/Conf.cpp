
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

bool keyExist(std::map<int, Connection*> connections, int key)
{
    std::map<int, Connection*>::iterator it = connections.find(key);
    return (it != connections.end());
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

void Config::creatPoll()
{
    struct epoll_event evlist[MAX_EVENT];

    int ep = epoll_create(1);
    if (ep == -1)
    {
        // Throw exception
        return;
    }

    for(size_t i = 0; i < _nembre_of_server; i++)
    {
         for (std::vector<std::string>::size_type y = 0; y < _server[i].getSock().size();  y++){

            struct epoll_event ev;
            ev.data.fd = _server[i].getSock()[y].second;
            ev.events = EPOLLIN;
            if (epoll_ctl(ep, EPOLL_CTL_ADD, _server[i].getSock()[y].second, &ev) == -1)
            {
                // Throw exception )
                return;
            }
         }

    }

    std::map<int, Connection*> connections;

    while(1)
    {
        deleteTimedoutSockets(connections, ep);
        int nbrReady = epoll_wait(ep, evlist, MAX_EVENT, -1);
        if(nbrReady < 0)
        {
            // Throw exception
            throw std::runtime_error("epoll_wait failed");
            // return;
        }
        for(int i = 0; i < nbrReady; i++)
        {
            int _fd = evlist[i].data.fd;
            if(evlist[i].events & EPOLLIN)
            {
                int server_fd = -1;
                server *tmp;
            
                for(size_t j = 0; j < _nembre_of_server; j++)
                {
                    for (std::vector<std::string>::size_type y = 0; y < _server[j].getSock().size();  y++){
                        if (_fd == _server[j].getSock()[y].second)
                        {
                            server_fd = _server[j].getSock()[y].second;
                            tmp = &_server[j];
                            break;
                        }
                    }

                }
                if (server_fd != -1)
                {
                    int new_fd = accept(server_fd, NULL,  0);
                    struct timeval timeout;
                    timeout.tv_sec = 10;
                    timeout.tv_usec = 0;
                    setsockopt(new_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

                    std::string logMessage = "[NEW CONNECTION] [SOCKET_FD: " + intToString(new_fd) + "]";
                    webServLog(logMessage, INFO);

                    struct epoll_event ev;
                    ev.data.fd = new_fd;
                    ev.events = EPOLLIN | EPOLLHUP | EPOLLERR;

                    if (epoll_ctl(ep, EPOLL_CTL_ADD, new_fd, &ev) == -1)
                    {

                        // Throw exception
                        throw std::runtime_error("epoll_ctl failed");
                        // return;
                    }
                    fcntl(new_fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
                    connections[new_fd] = new Connection(new_fd, tmp);
                    connections[new_fd]->setStartTime(time(NULL));
                }
                else
                {
                    if (connections[_fd]->sockRead() == -1)
                    {
                        close(_fd);
                        delete connections[_fd];
                        connections.erase(_fd);
                        epoll_ctl(ep, EPOLL_CTL_DEL, _fd, NULL);
                        std::string logMessage = "[CONNECTION CLOSED] [SOCKET_FD: " + intToString(_fd) + "]";
                        webServLog(logMessage, INFO);
                        continue;
                    }
                    if (connections[_fd]->readyToWrite())
                    {
                        struct epoll_event ev;
                        ev.data.fd = _fd;
                        ev.events = EPOLLOUT |  EPOLLIN  | EPOLLHUP | EPOLLERR;
                        if (epoll_ctl(ep, EPOLL_CTL_MOD, _fd, &ev) == -1)
                        {
                            // Throw exception
                            throw std::runtime_error("epoll_ctl failed");
                            // return ;
                        }
                    }
                }
            }
            if (evlist[i].events & EPOLLOUT)
            {
                if (connections[_fd]->sockWrite() == -1 || connections[_fd]->toBeClosed())
                {
                    bool keepAlive = connections[_fd]->keepAlive();
                    server *tmp = connections[_fd]->getServer();
                    delete connections[_fd];
                    connections.erase(_fd);

                    if (keepAlive)
                    {
                        try {
                            connections[_fd] = new Connection(_fd, tmp);
                            std::string logMessage = "[KEEP ALIVE] [SOCKET_FD: " + intToString(_fd) + "]";
                            webServLog(logMessage, INFO);
                        } catch (const std::bad_alloc &e) {
                            epoll_ctl(ep, EPOLL_CTL_DEL, _fd, NULL);
                            close(_fd);
                            std::string logMessage = "[CONNECTION CLOSED] [SOCKET_FD: " + intToString(_fd) + "]" + " [Memory allocation failed]";
                            webServLog(logMessage, ERROR);
                        }
                    }
                    else
                    {
                        epoll_ctl(ep, EPOLL_CTL_DEL, _fd, NULL);
                        close(_fd);
                        std::string logMessage = "[CONNECTION CLOSED] [SOCKET_FD: " + intToString(_fd) + "]";
                        webServLog(logMessage, INFO);
                    }
                    continue;
                }
            }
            if (evlist[i].events & EPOLLHUP || evlist[i].events & EPOLLERR)
            {
                close(_fd);
                delete connections[_fd];
                connections.erase(_fd);
                epoll_ctl(ep, EPOLL_CTL_DEL, _fd, NULL);
                std::string logMessage = "[CONNECTION CLOSED] [SOCKET_FD: " + intToString(_fd) + "]";
                webServLog(logMessage, INFO);
                continue;

            }
        }
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


