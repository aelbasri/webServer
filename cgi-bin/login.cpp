#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>

using namespace std;

string urlDecode(const string &encoded) {
    string decoded;
    char a, b;
    for (size_t i = 0; i < encoded.length(); i++) {
        if (encoded[i] == '%') {
            a = encoded[++i];
            b = encoded[++i];
            decoded += static_cast<char>((isxdigit(a) ? (a - (isdigit(a) ? '0' : (isupper(a) ? 'A' - 10 : 'a' - 10))) : 0) * 16 +
                                         (isxdigit(b) ? (b - (isdigit(b) ? '0' : (isupper(b) ? 'B' - 10 : 'b' - 10))) : 0));
        } else if (encoded[i] == '+') {
            decoded += ' ';
        } else {
            decoded += encoded[i];
        }
    }
    return decoded;
}

int main() {
    cout << "Content-Type: text/html\r\n\r\n";

    char *requestMethod = getenv("REQUEST_METHOD");
    string username, password;

    if (requestMethod && strcmp(requestMethod, "POST") == 0) {
        string input;
        getline(cin, input); 

        size_t usernamePos = input.find("username=");
        size_t passwordPos = input.find("&password=");

        if (usernamePos != string::npos && passwordPos != string::npos) {
            username = urlDecode(input.substr(usernamePos + 9, passwordPos - (usernamePos + 9)));
            password = urlDecode(input.substr(passwordPos + 10));
        }
    }

    string validUsername = "admin";
    string validPassword = "password";

    if (username == validUsername && password == validPassword) {
        cout << "<h1>Login Successful!</h1>";
        cout << "<p>Welcome, " << username << "!</p>";
    } else {
        cout << "<h1>Login Failed</h1>";
        cout << "<p>Invalid username or password.</p>";
    }
   
   return 0;
}