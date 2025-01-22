/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_data.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zel-khad <zel-khad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 10:57:03 by zel-khad          #+#    #+#             */
/*   Updated: 2025/01/22 14:08:25 by zel-khad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "error_pages.hpp"
#include "location.hpp"



class server : public error_pages
{
private:
    int     _nembre_of_location;
    location *_location;
    std::string _content;
    std::string _name;
    std::string _host;
    std::string _port;
    std::string _max_body_size;
public:
    void new_location();
    void Set_nembre_of_location(int _nembre_of_location);
    void Set_content(std::string __content);
    void Set_name(std::string __name);   
    void Set_host(std::string __host); 
    void Set_port(std::string __port);
    void Set_max_body_size(std::string __max_body_size); 
    
    int Get_nembre_of_location();
    std::string Get_content();
    std::string Get_name();   
    std::string Get_host(); 
    std::string Get_port();
    std::string Get_max_body_size();

    server();
    virtual ~server();

    void loadingDataserver(config_file *Conf);
    int CheckNumberOfLocation();
    
};