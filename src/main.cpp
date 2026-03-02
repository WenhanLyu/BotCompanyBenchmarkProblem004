#include <iostream>
#include <string>
#include <sstream>
#include "account.h"

// Normalize whitespace: multiple spaces become single space, trim leading/trailing
std::string normalizeWhitespace(const std::string& line) {
    std::stringstream ss(line);
    std::string word;
    std::string result;
    bool first = true;
    
    while (ss >> word) {
        if (!first) {
            result += " ";
        }
        result += word;
        first = false;
    }
    
    return result;
}

int main() {
    std::string line;
    AccountSystem accountSystem;
    
    // Main command loop - read line by line
    while (std::getline(std::cin, line)) {
        // Normalize whitespace
        std::string normalized = normalizeWhitespace(line);
        
        // Empty line - skip (no output)
        if (normalized.empty()) {
            continue;
        }
        
        // Extract the command (first word)
        std::stringstream ss(normalized);
        std::string command;
        ss >> command;
        
        // Handle quit and exit commands
        if (command == "quit" || command == "exit") {
            break;
        }
        
        // Handle su command
        if (command == "su") {
            std::string username, password;
            ss >> username;
            
            // Check if password is provided
            if (ss >> password) {
                // su with password
                if (accountSystem.login(username, password)) {
                    // Successful login - no output for su
                    continue;
                } else {
                    std::cout << "Invalid" << std::endl;
                }
            } else {
                // su without password - for M1, this is invalid
                std::cout << "Invalid" << std::endl;
            }
            continue;
        }
        
        // Handle logout command
        if (command == "logout") {
            if (accountSystem.logout()) {
                // Successful logout - no output
                continue;
            } else {
                std::cout << "Invalid" << std::endl;
            }
            continue;
        }
        
        // For now, all other commands are invalid
        std::cout << "Invalid" << std::endl;
    }
    
    return 0;
}
