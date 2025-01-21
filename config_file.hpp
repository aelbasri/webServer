/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_file.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zel-khad <zel-khad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 13:45:29 by zel-khad          #+#    #+#             */
/*   Updated: 2025/01/21 12:55:57 by zel-khad         ###   ########.fr       */
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


class config_file ;

class location
{
private:
    bool _autoindex;
    std::string _index;
    std::string _type_of_location;
    std::string _root_directory;
    std::string _allowed_methods;
    std::string _max_body_size;
    std::string _upload_dir;
    
public:
    location();
    virtual ~location();
};

class error_pages
{
private:
    std::string _Forbidden;
    std::string _NotFound;
    std::string _MethodNotAllowed;
    std::string _default;
public:
    error_pages();
    virtual ~error_pages();

};

class server : public error_pages , public location
{
private:
    std::string _name;
    std::string _host;
    std::string _port;
    std::string _max_body_size;
public:
    server();
    virtual ~server();

    void loadingDataserver(config_file Conf);
};



class config_file 
{
private:
    int     _nembre_of_server;
    std::string _fileContent;
    server *_server;
public:
    
    config_file();
    ~config_file();
  
    int get_nembre_of_server();
    std::string& setFileContent();
    int CheckNumberOfServer();
};






