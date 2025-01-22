/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_file.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zel-khad <zel-khad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 13:45:29 by zel-khad          #+#    #+#             */
/*   Updated: 2025/01/22 11:39:03 by zel-khad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <string>

// #define nullptr  0

class error_pages;
class config_file;
class location;
class server;


#include "location.hpp"
#include "server_data.hpp"
#include "error_pages.hpp"


using namespace std;


class config_file 
{
private:
    int     _nembre_of_server;
    std::string _fileContent;
    server *_server;
public:
    
    config_file();
    ~config_file();
  
    void getServer();
    void loadContentServer();
    int get_nembre_of_server();
    std::string& setFileContent();
    int CheckNumberOfServer();
};

void err();
