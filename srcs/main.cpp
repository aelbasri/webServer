#include "Conf.hpp"
#include <cmath>
#include <cstring>
#include <exception>
#include <iostream>
#include "log.hpp"

int main(int ac, char **av)
{
    try
    {
        if (ac != 2)
        {
            std::cerr << "USAGE: ./webserv CONFIG_FILE" << std::endl;
            return (1);
        }
        Config _Server(av[1]);

        webServLog("[WEBSERV IS STARTING ...]", INFO);
        _Server.loadContentServer();
        _Server.SetupServers();
     }
     catch (std::exception &e)
     {
        std::string error = "[L***A JAAAY! KHSER] [" + std::string(e.what()) + "] [ERRNO: " + std::string(strerror(errno)) + "]";
         webServLog(error, ERROR);
        return (1);
     }
    return (0);
}
