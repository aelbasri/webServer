/*#include "includes/configfile/server_data.hpp"*/
#include "Conf.hpp"
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
     catch (std::runtime_error &e)
     {
         std::cerr << "L***A JAAAY! KHSER " << e.what() << std::endl;
        return (1);
     }
    return (0);
}
