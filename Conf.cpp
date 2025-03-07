
#include "Conf.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Connection.hpp"
#include "configfile/server_data.hpp"
#include <stdlib.h>
#include "colors.hpp"
#include "log.hpp"

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
    loidingFile(file);
    _nembre_of_server = CheckNumberOfServer();
    _server =  new server[_nembre_of_server];
    
}

void err(){
    std::cerr << "error of open" << std::endl;
    exit(1);
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

void Config::loidingFile(std::string file){
    std::string s;
    std::string strCRLF = "\r\n";

    std::ifstream f(file.c_str());

    if (!f.is_open()) {
        err();
    }
    while (getline(f, s)){

        setFileContent() += s;
        if (!s.empty())
            setFileContent().push_back('\n');
    }
    f.close();
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

void Config::creatPoll()
{
    struct epoll_event evlist[MAX_EVENT];

    int ep = epoll_create(1);
    if (ep == -1)
    {
        // Throw exception
        std::cout << "epoll" << std::endl;
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
        int nbrReady = epoll_wait(ep, evlist, MAX_EVENT, -1);
        if(nbrReady < 0)
        {
            // Throw exception
            return;
        }
        // std::cout << "nbrReady: " << nbrReady << std::endl;
        for(int i = 0; i < nbrReady; i++)
        {
            int _fd = evlist[i].data.fd;
            if(evlist[i].events & EPOLLIN)
            {
                std::cout << "EPOLLIN ON SOCKET: " << _fd << std::endl;
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

                // std::cout << "aji nakhdo siservi" << std::endl;
                }
                if (server_fd != -1)
                {
                    int new_fd = accept(server_fd, NULL,  0);
                    struct timeval timeout;
                    timeout.tv_sec = 10;
                    timeout.tv_usec = 0;
                    setsockopt(new_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
                    /*std::cout << "new_fd: " << new_fd << std::endl;*/

                    std::string logMessage = "[NEW CONNECTION] [SOCKET_FD: " + intToString(new_fd) + "]";
                    webServLog(logMessage, INFO);

                    struct epoll_event ev;
                    ev.data.fd = new_fd;
                    // ev.events = EPOLLIN | EPOLLOUT;
                    ev.events = EPOLLIN | EPOLLHUP | EPOLLERR;
                    // evlist[i].data.fd = new_fd;
                    // evlist[i].events = EPOLLIN;

                    if (epoll_ctl(ep, EPOLL_CTL_ADD, new_fd, &ev) == -1)
                    {

                        // Throw exception
                        return;
                    }
                    fcntl(new_fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
                    connections[new_fd] = new Connection(new_fd, tmp);
                }
                else
                {
                    if (connections[_fd]->sockRead() == -1)
                    {
                        std::cout << "thanina mn 3adow lah" << std::endl;
                        close(_fd);
                        delete connections[_fd];
                        connections.erase(_fd);
                        // remove from epoll
                        epoll_ctl(ep, EPOLL_CTL_DEL, _fd, NULL);
                        std::string logMessage = "[CONNECTION CLOSED] [SOCKET_FD: " + intToString(_fd) + "]";
                        webServLog(logMessage, INFO);
                        continue;
                    }
                    if (connections[_fd]->readyToWrite())
                    {
                        std::cout << "Changing to EPOLLOUT ON SOCKET: " << _fd << std::endl;
                        struct epoll_event ev;
                        ev.data.fd = _fd;
                        ev.events = EPOLLOUT |  EPOLLIN  | EPOLLHUP | EPOLLERR;
                        if (epoll_ctl(ep, EPOLL_CTL_MOD, _fd, &ev) == -1)
                        {
                            // Throw exception
                            return ;
                        }
                    }
                }
            }
            if (evlist[i].events & EPOLLOUT)
            {
                // std::cout << "EPOLLOUT ON SOCKET: " << _fd << std::endl;
                // if (connections[_fd]->_response.getProgress() == FINISHED)
                // {
                //         close(_fd);
                //     std::cout << "khas dkhol hna" << std::endl;
                // }
                if (connections[_fd]->sockWrite() == -1 || connections[_fd]->toBeClosed())
                {
                    // std::cout << "kaan hna" << std::endl;
                    // exit(10);
                    bool keepAlive = connections[_fd]->keepAlive();
                    server *tmp = connections[_fd]->getServer();
                    delete connections[_fd];
                    connections.erase(_fd);

                    if (!keepAlive) // if Connection: close
                    {
                        // remove from epoll
                        epoll_ctl(ep, EPOLL_CTL_DEL, _fd, NULL);
                        std::string logMessage = "[CONNECTION CLOSED] [SOCKET_FD: " + intToString(_fd) + "]";
                        webServLog(logMessage, INFO);
                        close(_fd);
                    }
                    else
                    {
                        std::string logMessage = "[KEEP ALIVE] [SOCKET_FD: " + intToString(_fd) + "]";
                        webServLog(logMessage, INFO);
                        connections[_fd] = new Connection(_fd, tmp);
                    }
                    continue;
                }
            }
            if (evlist[i].events & EPOLLHUP || evlist[i].events & EPOLLERR)
            {
                std::cout << "EPOLLHUP OR EPOLLERR ON SOCKET: " << _fd << std::endl;
                close(_fd);
                delete connections[_fd];
                connections.erase(_fd);
                // remove from epoll
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
        exit(1);
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
            exit(1);
        }
        std::cout << "Directory removed: " << path << std::endl;
    }

    // Create the directory
    if (mkdir(path.c_str(), 0777) != 0) {
        std::cerr << "Failed to create directory: " << path << " - " << strerror(errno) << std::endl;
        exit(1);
    }
    std::cout << "Directory created: " << path << std::endl;
}

int Config::SetupServers()
{
    std::cout << " _number_of_servers : ============"<< _nembre_of_server << std::endl; 
    for (size_t i = 0; i < _nembre_of_server ; i++)
    {
        /*std::cout << "i: ============"<< i << std::endl; */
        /*std::cout << getpid() << std::endl;*/
        if (_server[i].run() == -1)
            exit(1);
    }
    removeAndRecreateDirectory(UPLOAD_DIRECTORY);
    creatPoll();
    return (0);
}


