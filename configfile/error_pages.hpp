/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_pages.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zel-khad <zel-khad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 10:50:32 by zel-khad          #+#    #+#             */
/*   Updated: 2025/01/22 11:39:46 by zel-khad         ###   ########.fr       */
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

};