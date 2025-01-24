/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_pages.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zel-khad <zel-khad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 10:50:26 by zel-khad          #+#    #+#             */
/*   Updated: 2025/01/24 20:48:21 by zel-khad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error_pages.hpp"

error_pages::error_pages(){
    _default = 
    "<!DOCTYPE html>\n"
    "<html>\n"
    "<head>\n"
    "<title>Error</title>\n"
    "<style>\n"
    "html { color-scheme: light dark; }\n"
    "body { width: 35em; margin: 0 auto;\n"
    "font-family: Tahoma, Verdana, Arial, sans-serif; }\n"
    "</style>\n"
    "</head>\n"
    "<body>\n"
    "<h1>An error occurred.</h1>\n"
    "<p>Sorry, the page you are looking for is currently unavailable.<br/>\n"
    "Please try again later.</p>\n"
    "<p>If you are the system administrator of this resource then you should check\n"
    "the error log for details.</p>\n"
    "<p><em>Faithfully yours, zaki.</em></p>\n"
    "</body>\n"
    "</html>";
}

error_pages::~error_pages(){}


void error_pages::SetDefault(std::string __default){
    _default = __default;
}

std::string error_pages::GetDefault(){
    return _default;
}

void error_pages::SetErr(std::map<std::string , std::string> __err){
    _err = __err;
}

std::map<std::string , std::string> error_pages::GetErr(){
    return(_err);
}