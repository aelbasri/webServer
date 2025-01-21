/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_file.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zel-khad <zel-khad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 13:45:29 by zel-khad          #+#    #+#             */
/*   Updated: 2025/01/21 16:21:52 by zel-khad         ###   ########.fr       */
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

class server : public error_pages
{
private:
    int     _nembre_of_location;
    config_file *_location;
    std::string _content;
    std::string _name;
    std::string _host;
    std::string _port;
    std::string _max_body_size;
public:
    void Set_content(std::string __content);
    void Set_name(std::string __name);   
    void Set_host(std::string __host); 
    void Set_port(std::string __port);
    void Set_max_body_size(std::string __max_body_size); 
    
    std::string Get_content();
    std::string Get_name();   
    std::string Get_host(); 
    std::string Get_port();
    std::string Get_max_body_size();

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
  
    void getServer();
    void loadContentServer();
    int get_nembre_of_server();
    std::string& setFileContent();
    int CheckNumberOfServer();
};






