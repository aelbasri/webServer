#include "Connection.hpp"

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
        for (int i = 0; i < toDelete.size(); i++)
        {
            int _fd = toDelete[i]->getSocket();
            delete toDelete[i];
            connections.erase(_fd);
        }
}