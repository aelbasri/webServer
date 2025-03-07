/*#include "configfile/server_data.hpp"*/
#include "Conf.hpp"
#include <exception>
#include <iostream>
#include "log.hpp"

int main(int ac, char **av)
{
    try
    {
        if (ac != 2)
            throw std::runtime_error("Invalid argument size");
        Config _Server(av[1]);

        webServLog("Server is starting...", INFO);
        _Server.loadContentServer();
        _Server.SetupServers();
     }
     catch (std::runtime_error &e)
     {
         std::cout << "L***A JAAAY!: " << e.what() << std::endl;
     }
    return (0);
}
