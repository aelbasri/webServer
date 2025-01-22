/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_pages.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zel-khad <zel-khad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 10:50:32 by zel-khad          #+#    #+#             */
/*   Updated: 2025/01/22 17:45:51 by zel-khad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>

#include "error_pages.hpp"

class error_pages
{
private:
    std::string _Forbidden;
    std::string _NotFound;
    std::string _MethodNotAllowed;
    std::string _default;
public:
    error_pages();
    virtual ~error_pages();


    void SetForbiden(std::string __Forbidden);
    void SetNotfound(std::string __NotFound);
    void SetMethodNotAllowed(std::string __MethodNotAllowed);
    void SetDefault(std::string __default);
    

    std::string GetForbiddent();
    std::string GetNotFound();
    std::string GetMethodNotAllowed();
    std::string GetDefault();
};