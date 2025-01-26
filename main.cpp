#include "configfile/server_data.hpp"
#include "Conf.hpp"
#include <iostream>

int main()
{
    try
    {
        Config _Server("config_file.yaml");

        _Server.loadContentServer();
        _Server.SetupServers();
        // exit(0);

     }
     catch (std::runtime_error e)
     {
         std::cout << "L***A JAAAY!: " << e.what();
     }
    return (0);
}
