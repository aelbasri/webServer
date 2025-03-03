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
#include "../Request.hpp"
#include "../Response.hpp"
#include "cgi_data.hpp"

#include <clocale>
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
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <sys/epoll.h>

class CGI;

#define MAX_EVENT 5
#define FILE_PATH "./assets/page.html"
#define UPLOAD_DIRECTORY "./uploads/"

class server : public error_pages 
{
private:
    int     _number_of_location;
    std::string _content;
    std::string _name;
    std::string _host;
    long long _max_body_size;

    size_t _indixL;
    int     _NPort;

    // std::vector<std::string> _port;
    std::vector<CGI> _CGI;
    location *_location;

    std::vector<std::pair<std::string, int> >  _sock;
    struct addrinfo hints;
    struct addrinfo *res ,*p;
    int addI;

public:
    void new_location();
    void Set_number_of_location(int _number_of_location);
    void Set_content(std::string __content);
    void Set_name(std::string __name);   
    void Set_host(std::string __host); 
    // void Set_port(std::vector<std::string> __port);
    void Set_max_body_size(long long __max_body_size);
    void setSock(std::string port,int sock);
    void setRes(struct addrinfo* newRes);
    void setP(struct addrinfo* newP);
    void setAddI(int newAddI);\
    void SetCgi(std::vector<CGI> __cgi);

    
    int Get_number_of_location();
    std::string Get_content();
    std::string Get_name();   
    std::string Get_host(); 
    //  std::vector<std::string> Get_port() { retru};
    long long Get_max_body_size();
    std::vector<CGI> GetCgi();

    std::vector<std::pair<std::string, int> >  getSock() ;
    struct addrinfo &getHints() ;
    void setHints( struct addrinfo& newHints);
    struct addrinfo* &getRes();
    struct addrinfo* &getP();
    int getAddI();

    server();
    virtual ~server();
    server(const server &server)
    {
        *this = server;
    }

    server& operator=(const server &server);

    location* GetLocations() const
    {
        return (_location);
    }

    void Getlocation();
    void LoidingAllowedMethods(std::vector<std::string> lines,size_t &i);
    void loadingLocationContent(std::vector<std::string> lines,size_t &i);
    void loadingDataserver();
    int CheckNumberOfLocation();
    void loadingErrorIndex(std::vector<std::string> lines, size_t &i);
    void loadingCgiContent(std::vector<std::string> lines,size_t &i);
    int run();

    public:
        class InternalServerError : public std::exception
        {
            public :
                const char *what() const throw();
        };
};

void handle_request(int fd);
