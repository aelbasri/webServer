#include "server.hpp"
#include <iostream>

int main()
{
    Server server;

    std::cout << getpid() << std::endl;
    if (server.run() == -1)
        exit(1);
    server.printRequest();
    return (0);
}
