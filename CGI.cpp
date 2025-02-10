#include <iostream>
#include <cstdio>

int main() {
    const char* command = "python3 test.py"; 

    FILE* pipe = popen(command, "r");
    if (!pipe) {
        std::cerr << "Error executing command." << std::endl;
        return 1;
    }

    char buffer[128];
    std::string result = "";
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }

    pclose(pipe);

    std::cout << "Content-Type: text/html\r\n\r\n";
    std::cout << "<html><body><h1>Script Output</h1><pre>" << result << "</pre></body></html>";

    return 0;
}