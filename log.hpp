#pragma once

#include "colors.hpp"
#include <iostream>
#include <ctime>

enum LogLevel {
    INFO,
    ERROR
};

void webServLog(const std::string& message, LogLevel level);
