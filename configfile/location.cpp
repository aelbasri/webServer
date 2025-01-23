/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zel-khad <zel-khad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 10:52:58 by zel-khad          #+#    #+#             */
/*   Updated: 2025/01/23 16:09:36 by zel-khad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "location.hpp"

location::location(){}

location::~location(){}


void location::SetType_of_location(std::string __type_of_location){
    _type_of_location = __type_of_location;
}

void location::SetIndex(std::string __index){
    _index = __index;
}

void location::SetRoot_directory(std::string __root_directory){
    _root_directory = __root_directory;
}

void location::SetAllowed_methods(int i, std::string target){
    _allowed_methods[i] = target;
}

std::string location::GetAllowed_methods(int i){
    return _allowed_methods[i];
}

void location::SetMax_body_size(long long __max_body_size){
    _max_body_size = __max_body_size;
}


std::string location::GetType_of_location(){
    return _type_of_location;
}

std::string location::GetIndex(){
    return _index;
}

std::string location::GetRoot_directory(){
    return _root_directory;
}

std::string location::GetUpload_dir(){
    return _upload_dir;
}

long long location::GetMax_body_size(){
    return _max_body_size;
}



// void location::loadContenLocation(){
    
// }