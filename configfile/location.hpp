/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zel-khad <zel-khad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 10:53:03 by zel-khad          #+#    #+#             */
/*   Updated: 2025/01/23 11:31:53 by zel-khad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "config_file.hpp"

class location
{
private:
    bool _autoindex;
    std::string _index;
    std::string _type_of_location;
    std::string _root_directory;
    std::string _allowed_methods;
    long long _max_body_size;
    std::string _upload_dir;
    
public:
    location();
    virtual ~location();
};