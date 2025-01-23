/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_data.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zel-khad <zel-khad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 10:57:03 by zel-khad          #+#    #+#             */
/*   Updated: 2025/01/23 19:48:57 by zel-khad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "error_pages.hpp"
#include "location.hpp"



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
public:
    void new_location();
    void Set_nembre_of_location(int _nembre_of_location);
    void Set_content(std::string __content);
    void Set_name(std::string __name);   
    void Set_host(std::string __host); 
    void Set_port(std::string __port);
    void Set_max_body_size(long long __max_body_size); 
    
    int Get_nembre_of_location();
    std::string Get_content();
    std::string Get_name();   
    std::string Get_host(); 
    std::string Get_port();
    long long Get_max_body_size();

    server();
    virtual ~server();



    void Getlocation();
    void LoidingAllowedMethods(std::vector<std::string> lines,size_t &i);
    void loadingLocationContent(std::vector<std::string> lines,size_t &i);
    void loadingDataserver(config_file *Conf);
    int CheckNumberOfLocation();
    void loadingErrorIndex(std::vector<std::string> lines, size_t &i);
};