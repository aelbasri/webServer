#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>

int main()
{
    struct addrinfo hints;
    struct addrinfo *res ,*p;
    int yes = 1;
    int sockFd;
    std::string port = "3301"; 
    int addI;

    memset(&hints, 0,  sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if ((addI = getaddrinfo(NULL, port.c_str(), &hints, &res)) != 0)
    {
        std::cerr << gai_strerror(addI) << std::endl;
        return 1;
    }

    for(p = res; res; p = p->ai_next)
    {
        if ((sockFd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
            continue;
        if ((setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) == -1)
        {
            perror("setsockopt");
            exit(1);
        }
        //bind
        if (bind(sockFd, res->ai_addr, res->ai_addrlen) == -1)
            continue;
        break;
    }
    if (!p)
    {
        perror("bind failed");
        exit(1);
    }

    listen(sockFd, 10);
    while(1)
    {

    }
}