/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zel-khad <zel-khad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 10:53:03 by zel-khad          #+#    #+#             */
/*   Updated: 2025/01/24 16:59:31 by zel-khad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../server.hpp"

class location
{
private:
    std::string _type_of_location;
    std::string _index;
    std::string _root_directory;
    std::string _upload_dir;
    std::vector<std::string>  _allowed_methods;
    long long _max_body_size;
    
public:
    location();
    virtual ~location();


    void SetType_of_location(std::string __type_of_location);
    void SetIndex(std::string __index);
    void SetRoot_directory(std::string __root_directory);
    void SetAllowed_methods(std::vector<std::string> &target);
    void SetUpload_dir(std::string __upload_dir);
    void SetMax_body_size(long long __max_body_size);


    std::vector<std::string> GetAllowed_methods();
    std::string GetType_of_location();    
    std::string GetIndex();
    std::string GetRoot_directory();    
    std::string GetUpload_dir();
    long long GetMax_body_size();
    
};