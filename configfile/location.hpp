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

#include "../Conf.hpp"

class location
{
private:
    std::vector<std::string> _index;
    std::vector<std::string>  _allowed_methods;
    std::string _type_of_location;
    std::string _root_directory;
    std::string _upload_dir;
    std::string _rewrite;
    long long _max_body_size;
    bool directoryListing;
    
public:
    location();
    virtual ~location();


    void SetType_of_location(std::string __type_of_location);
    void SetIndex(std::vector<std::string> __index);
    void SetAllowed_methods(std::vector<std::string> target);
    void SetRoot_directory(std::string __root_directory);
    void SetUpload_dir(std::string __upload_dir);
    void SetMax_body_size(long long __max_body_size);
    void SetRewrite(std::string __rewrite);
    void SetDirectoryListing(bool __directoryListing);

    std::vector<std::string> GetAllowed_methods() const;
    std::vector<std::string> &GetIndex() ;
    std::string GetType_of_location() const;    
    std::string GetRoot_directory() const;    
    std::string GetUpload_dir() const;
    long long GetMax_body_size() const;
    std::string  GetRewrite() const;
    bool GetDirectoryListing() const;
    
};