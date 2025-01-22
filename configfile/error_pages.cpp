/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_pages.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zel-khad <zel-khad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 10:50:26 by zel-khad          #+#    #+#             */
/*   Updated: 2025/01/22 17:47:25 by zel-khad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error_pages.hpp"

error_pages::error_pages(){}

error_pages::~error_pages(){}

void error_pages::SetForbiden(std::string __Forbidden){
    _Forbidden = __Forbidden;
}

void error_pages::SetNotfound(std::string __NotFound){
    _NotFound = __NotFound;
}

void error_pages::SetMethodNotAllowed(std::string __MethodNotAllowed){
    _MethodNotAllowed = __MethodNotAllowed;
}

void error_pages::SetDefault(std::string __default){
    _default = __default;
}

std::string error_pages::GetForbiddent(){
    return _Forbidden;
}

std::string error_pages::GetNotFound(){
    return _NotFound;
}

std::string error_pages::GetMethodNotAllowed(){
    return _MethodNotAllowed;
}

std::string error_pages::GetDefault(){
    return _default;
}