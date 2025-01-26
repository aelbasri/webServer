/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_data.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zel-khad <zel-khad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 10:57:03 by zel-khad          #+#    #+#             */
/*   Updated: 2025/01/25 11:22:46 by zel-khad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "error_pages.hpp"
#include "location.hpp"

#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

class server : public error_pages
{
private:
    size_t _indixL;
    int     _nembre_of_location;
    location *_location;
    std::string _content;
    std::string _name;
    std::string _host;
    std::string _port;
    long long _max_body_size;

    int         _sock;
    struct addrinfo hints;
    struct addrinfo *res ,*p;
    int addI;

public:
    void new_location();
    void Set_nembre_of_location(int _nembre_of_location);
    void Set_content(std::string __content);
    void Set_name(std::string __name);   
    void Set_host(std::string __host); 
    void Set_port(std::string __port);
    void Set_max_body_size(long long __max_body_size);
    void setSock(int sock);
    void setRes(struct addrinfo* newRes);
    void setP(struct addrinfo* newP);
    void setAddI(int newAddI);
    // void Set

    
    int Get_nembre_of_location();
    std::string Get_content();
    std::string Get_name();   
    std::string Get_host(); 
    std::string Get_port();
    long long Get_max_body_size();

    int getSock() ;
    struct addrinfo &getHints() ;
    void setHints( struct addrinfo& newHints);
    struct addrinfo* &getRes();
    struct addrinfo* &getP();
    int getAddI();

    server();
    virtual ~server();



    void Getlocation();
    void LoidingAllowedMethods(std::vector<std::string> lines,size_t &i);
    void loadingLocationContent(std::vector<std::string> lines,size_t &i);
    void loadingDataserver();
    int CheckNumberOfLocation();
    void loadingErrorIndex(std::vector<std::string> lines, size_t &i);
};


