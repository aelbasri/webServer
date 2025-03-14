#include "Conf.hpp"
#include "Connection.hpp"
#include "server_data.hpp"
#include <cerrno>
#include <stdexcept>
#include <stdlib.h>
#include "colors.hpp"
#include "log.hpp"

bool safelyAddToEpoll(int epollFd, int fd, uint32_t events) {
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = events;
    
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        std::string errorMsg = "epoll_ctl ADD failed for fd " + intToString(fd) + 
                              ": " + std::string(strerror(errno));
        webServLog(errorMsg, ERROR);
        return false;
    }
    return true;
}

bool safelyModifyInEpoll(int epollFd, int fd, uint32_t events) {
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = events;
    
    if (epoll_ctl(epollFd, EPOLL_CTL_MOD, fd, &ev) == -1) {
        if (errno == ENOENT) {
            // If descriptor is not found try to add it instead
            return safelyAddToEpoll(epollFd, fd, events);
        } else {
            std::string errorMsg = "epoll_ctl MOD failed for fd " + intToString(fd) + 
                                  ": " + std::string(strerror(errno));
            webServLog(errorMsg, ERROR);
            return false;
        }
    }
    return true;
}

bool safelyRemoveFromEpoll(int epollFd, int fd) {
    if (epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL) == -1) {
        if (errno != EBADF && errno != ENOENT) {
            std::string errorMsg = "epoll_ctl DEL failed for fd " + intToString(fd) + 
                                  ": " + std::string(strerror(errno));
            webServLog(errorMsg, ERROR);
            return false;
        }
        // EBADF (bad file descriptor) or ENOENT (not found) are acceptable when removing
    }
    return true;
}

bool safelyCloseFd(int fd) {
    if (fd >= 0) {
        if (close(fd) == -1) {
            std::string errorMsg = "close failed for fd " + intToString(fd) + 
                                  ": " + std::string(strerror(errno));
            webServLog(errorMsg, ERROR);
            return false;
        }
    }
    return true;
}


bool KeyExists(std::map<int, Connection*> connections, int key)
{
    std::map<int, Connection*>::iterator it = connections.find(key);
    return (it != connections.end());
}

// --------------------
int Config::createEpollInstance()
{
    int ep = epoll_create(1);
    if (ep == -1) {
        throw std::runtime_error("epoll_create failed");
    }
    return ep;
}

void Config::addSocketsToEpoll(int ep)
{
    for (size_t i = 0; i < _nembre_of_server; i++) {
        for (std::vector<std::string>::size_type y = 0; y < _server[i].getSock().size(); y++) {
            struct epoll_event ev;
            ev.data.fd = _server[i].getSock()[y].second;
            ev.events = EPOLLIN;
            if (!safelyAddToEpoll(ep, _server[i].getSock()[y].second, EPOLLIN)) {
                safelyCloseFd(_server[i].getSock()[y].second);
                throw std::runtime_error("epoll_ctl failed 1");
            }
        }
    }
}

void Config::handleNewConnection(int ep, int server_fd, server* tmp, std::map<int, Connection*>& connections)
{
    int new_fd = accept(server_fd, NULL, 0);
    if (new_fd == -1) {
        throw std::runtime_error("accept failed");
    }

    struct timeval timeout;
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;
    if (setsockopt(new_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1)
        throw std::runtime_error("setsockopt failed");

    std::string logMessage = "[NEW CONNECTION] [SOCKET_FD: " + intToString(new_fd) + "]";
    webServLog(logMessage, INFO);

    struct epoll_event ev;
    ev.data.fd = new_fd;
    ev.events = EPOLLIN | EPOLLHUP | EPOLLERR;

    if (!safelyAddToEpoll(ep, new_fd, EPOLLIN | EPOLLHUP | EPOLLERR)) {
        safelyCloseFd(new_fd);
        throw std::runtime_error("epoll_ctl failed 1");
    }

    if (fcntl(new_fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1)
        throw std::runtime_error("fcntl failed");
    connections[new_fd] = new Connection(new_fd, tmp);
    connections[new_fd]->setStartTime(time(NULL));
}

int Config::handleReadEvent(int ep, int fd, std::map<int, Connection*>& connections)
{
    if (connections[fd]->sockRead() == -1) {
        delete connections[fd];
        connections.erase(fd);
        if (!safelyRemoveFromEpoll(ep, fd))
        {
            safelyCloseFd(fd);
            throw std::runtime_error("epoll_ctl failed 1");
        }
        safelyCloseFd(fd);
        std::string logMessage = "[CONNECTION CLOSED] [SOCKET_FD: " + intToString(fd) + "]";
        webServLog(logMessage, INFO);
        return (-1);
    }

    if (connections[fd]->readyToWrite()) {
        struct epoll_event ev;
        ev.data.fd = fd;
        ev.events = EPOLLOUT | EPOLLIN | EPOLLHUP | EPOLLERR;
        if (!safelyModifyInEpoll(ep, fd, EPOLLOUT | EPOLLIN | EPOLLHUP | EPOLLERR))
        {
            safelyCloseFd(fd);
            throw std::runtime_error("epoll_ctl failed 1");
        }

    }
    return (0);
}

int Config::handleWriteEvent(int ep, int fd, std::map<int, Connection*>& connections)
{
    if (connections[fd]->sockWrite() == -1 || connections[fd]->toBeClosed()) {
        bool keepAlive = connections[fd]->keepAlive();
        server* tmp = connections[fd]->getServer();
        delete connections[fd];
        connections.erase(fd);

        if (keepAlive) {
            try {
                connections[fd] = new Connection(fd, tmp);
                connections[fd]->setStartTime(time(NULL));
                std::string logMessage = "[KEEP ALIVE] [SOCKET_FD: " + intToString(fd) + "]";
                webServLog(logMessage, INFO);
            } catch (const std::bad_alloc& e) {
                if (!safelyRemoveFromEpoll(ep, fd))
                {
                    safelyCloseFd(fd);
                    throw std::runtime_error("epoll_ctl failed 1");
                }
                safelyCloseFd(fd);
                std::string logMessage = "[CONNECTION CLOSED] [SOCKET_FD: " + intToString(fd) + "]" + " [Memory allocation failed]";
                webServLog(logMessage, ERROR);
                return (-1);
            }
        } else {
            if (!safelyRemoveFromEpoll(ep, fd))
            {
                safelyCloseFd(fd);
                throw std::runtime_error("epoll_ctl failed 1");
            }
            safelyCloseFd(fd);
            std::string logMessage = "[CONNECTION CLOSED] [SOCKET_FD: " + intToString(fd) + "]";
            webServLog(logMessage, INFO);
        }
    }
    return (0);
}

int Config::handleErrorEvent(int ep, int fd, std::map<int, Connection*>& connections)
{
    delete connections[fd];
    connections.erase(fd);
    if (!safelyRemoveFromEpoll(ep, fd))
    {
        safelyCloseFd(fd);
        throw std::runtime_error("epoll_ctl failed 1");
    }
    safelyCloseFd(fd);
    std::string logMessage = "[CONNECTION CLOSED] [SOCKET_FD: " + intToString(fd) + "]";
    webServLog(logMessage, INFO);
    return (0);
}

void Config::pollLoop(int ep, std::map<int, Connection*>& connections)
{
    struct epoll_event evlist[MAX_EVENT];

    while (1) {
        deleteTimedoutSockets(connections, ep);
        int nbrReady = epoll_wait(ep, evlist, MAX_EVENT, -1);
        if (nbrReady < 0) {
            throw std::runtime_error("epoll_wait failed");
        }

        for (int i = 0; i < nbrReady; i++) {
            int fd = evlist[i].data.fd;
            if (evlist[i].events & EPOLLIN) {
                // check if event happened on server socket
                int server_fd = -1;
                server* tmp = nullptr;
                for (size_t j = 0; j < _nembre_of_server; j++) {
                    for (std::vector<std::string>::size_type y = 0; y < _server[j].getSock().size(); y++) {
                        if (fd == _server[j].getSock()[y].second) {
                            server_fd = _server[j].getSock()[y].second;
                            tmp = &_server[j];
                            break;
                        }
                    }
                }

                if (server_fd != -1) {
                    handleNewConnection(ep, server_fd, tmp, connections);
                    continue;
                } else {
                    if (!KeyExists(connections, fd))
                        continue;
                    if (handleReadEvent(ep, fd, connections) == -1)
                        continue;
                }
            }

            if (evlist[i].events & EPOLLOUT) {
                if (!KeyExists(connections, fd))
                    continue;
                if (handleWriteEvent(ep, fd, connections) == -1)
                    continue;
            }

            if (evlist[i].events & EPOLLHUP || evlist[i].events & EPOLLERR) {
                if (!KeyExists(connections, fd))
                    continue;
                handleErrorEvent(ep, fd, connections);
            }
        }
    }
}

void Config::creatPoll() {
    int ep = createEpollInstance();
    addSocketsToEpoll(ep);

    std::map<int, Connection*> connections;
    pollLoop(ep, connections);
}
