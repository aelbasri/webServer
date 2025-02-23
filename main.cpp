#include "configfile/server_data.hpp"
#include "Conf.hpp"
#include <exception>
#include <iostream>
#include "log.hpp"

int main()
{
    try
    {
        Config _Server("config_file.yaml");

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
