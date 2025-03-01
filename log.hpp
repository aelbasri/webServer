#pragma once

#include "colors.hpp"
#include <iostream>
#include <ctime>

enum LogLevel {
    INFO,
    WARNING,
    ERROR
};

void webServLog(const std::string& message, LogLevel level);
