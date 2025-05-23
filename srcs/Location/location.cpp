/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zel-khad <zel-khad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 10:52:58 by zel-khad          #+#    #+#             */
/*   Updated: 2025/01/24 16:59:26 by zel-khad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "location.hpp"

location::location():_upload_dir("./UPLOAD_DIRDFL/"), directoryListing(false){}

location::~location(){}

void location::SetType_of_location(std::string __type_of_location){
    _type_of_location = __type_of_location;
}

void location::SetIndex(std::vector<std::string> __index){
    _index = __index;
}

void location::SetRoot_directory(std::string __root_directory){
    _root_directory = __root_directory;
}

void location::SetAllowed_methods(std::vector<std::string> target){
    _allowed_methods = target;
}

std::vector<std::string>  location::GetAllowed_methods() const{
    return _allowed_methods;
}

void location::SetUpload_dir(std::string __Upload_dir){
    _upload_dir = __Upload_dir;
}

void location::SetMax_body_size(long long __max_body_size){
    _max_body_size = __max_body_size;
}

void location::SetRewrite(std::string __rewrite){
    _rewrite = __rewrite;
}

void location::SetDirectoryListing(bool __directoryListing){
    directoryListing = __directoryListing;
}

std::string location::GetType_of_location() const{
    return _type_of_location;
}

std::vector<std::string> &location::GetIndex() {
    return _index;
}

std::string location::GetRoot_directory() const{
    return _root_directory;
}

std::string location::GetUpload_dir() const{
    return _upload_dir;
}

long long location::GetMax_body_size() const{
    return _max_body_size;
}

std::string location::GetRewrite() const{
    return _rewrite;
}

bool location::GetDirectoryListing() const{
    return directoryListing;
}