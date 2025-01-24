/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_pages.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zel-khad <zel-khad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 10:50:32 by zel-khad          #+#    #+#             */
/*   Updated: 2025/01/24 19:36:32 by zel-khad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once


#include "error_pages.hpp"

#include <iostream>
#include <map>

class error_pages
{
private:
    std::map<std::string , std::string> _err;
    std::string _default;
public:
    error_pages();
    virtual ~error_pages();


    void SetErr(std::map<std::string , std::string> __err);
    void SetDefault(std::string __default);
    

    std::map<std::string , std::string> GetErr();
    std::string GetDefault();
};