#include "log.hpp"

void webServLog(const std::string& message, LogLevel level)
{
    std::time_t now = std::time(NULL);
    char timestamp[20];
    std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

    std::string logMessage = "[" + std::string(timestamp) + "] [" + (level == INFO ? GREEN "INFO" : RED "ERROR") + RESET "] " + message;

    if (level == INFO) {
        std::cout << logMessage << std::endl;
    } else {
        std::cerr << logMessage << std::endl;
    }
}
