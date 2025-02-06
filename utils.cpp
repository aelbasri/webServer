#include "Response.hpp"
#include "configfile/location.hpp"

std::map<std::string, std::string> initializeMimeTypes() {
    std::map<std::string, std::string> mimeTypes;
    mimeTypes[".html"] = "text/html; charset=UTF-8";
    mimeTypes[".htm"] = "text/html; charset=UTF-8";
    mimeTypes[".css"] = "text/css";
    mimeTypes[".js"] = "application/javascript";
    mimeTypes[".json"] = "application/json";
    mimeTypes[".xml"] = "application/xml";
    mimeTypes[".txt"] = "text/plain";
    mimeTypes[".jpg"] = "image/jpeg";
    mimeTypes[".jpeg"] = "image/jpeg";
    mimeTypes[".png"] = "image/png";
    mimeTypes[".gif"] = "image/gif";
    mimeTypes[".svg"] = "image/svg+xml";
    mimeTypes[".ico"] = "image/vnd.microsoft.icon";
    mimeTypes[".webp"] = "image/webp";
    mimeTypes[".mp4"] = "video/mp4";
    mimeTypes[".mpeg"] = "video/mpeg";
    mimeTypes[".wav"] = "audio/wav";
    mimeTypes[".mp3"] = "audio/mpeg";
    mimeTypes[".ogg"] = "audio/ogg";
    mimeTypes[".pdf"] = "application/pdf";
    mimeTypes[".zip"] = "application/zip";
    mimeTypes[".tar"] = "application/x-tar";
    mimeTypes[".gz"] = "application/gzip";
    mimeTypes[".csv"] = "text/csv";
    return mimeTypes;
}

std::string getMimeType(const std::string& filename) {
    static const std::map<std::string, std::string> mimeTypes = initializeMimeTypes();

    size_t dotPos = filename.rfind('.');
    if (dotPos != std::string::npos) {
        std::string extension = filename.substr(dotPos);
        std::map<std::string, std::string>::const_iterator it = mimeTypes.find(extension);
        if (it != mimeTypes.end()) {
            return it->second;
        }
    }
    return "application/octet-stream";
}

bool methodAllowed(const std::string& method, const std::vector<std::string>& allowedMethods) {
    for (size_t i = 0; i < allowedMethods.size(); i++) {
        if (method == allowedMethods[i]) {
            return true;
        }
    }
    return false;
}

location* getLocationMatch(std::string target, location *locations, int size) {
    location *best_match = nullptr;
    size_t best_match_length = 0;

    // std::cout << "size: " << size << std::endl;

    for (int i = 0; i < size; i++) {
        std::string locType = locations[i].GetType_of_location();
        // std::cout << "Lets compare: " << locType << " with " << target << std::endl;
        if (locType == target) {
            return &locations[i];
        }
        if (target.find(locType) == 0 && locType.length() > best_match_length) {
            best_match = &locations[i];
            best_match_length = locType.length();
        }
    }
    // std::cout << std::endl;

    if (best_match == nullptr)
    {
        // std::cout << "walo abro" << std::endl;
        for (int i = 0; i < size; i++) {
            if (locations[i].GetType_of_location() == "/") {
                best_match = &locations[i];
                break;
            }
        }
    }
    return (best_match);
}

