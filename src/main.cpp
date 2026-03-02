#include <iostream>
#include <string>
#include <sstream>

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
        
        // For now, all other commands are invalid
        std::cout << "Invalid" << std::endl;
    }
    
    return 0;
}
