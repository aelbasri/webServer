#include "log.hpp"

void webServLog(const std::string& message, LogLevel level)
{
    std::time_t now = std::time(NULL);
    char timestamp[20];
    std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

    std::string logMessage = "[" + std::string(timestamp) + "] [";
    if (level == INFO)
        logMessage += GREEN "INFO";
    else if (level == ERROR)
        logMessage += RED "ERROR";
    else
        logMessage += YELLOW "WARNING";
    logMessage += RESET "] " + message;

    if (level == INFO) {
        std::cout << logMessage << std::endl;
    } else {
        std::cerr << logMessage << std::endl;
    }
}
