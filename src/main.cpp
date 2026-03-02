#include <iostream>
#include <string>

int main() {
    std::string command;
    
    // Main command loop
    while (std::cin >> command) {
        // Handle quit and exit commands
        if (command == "quit" || command == "exit") {
            break;
        }
        
        // Placeholder for other commands
        std::cout << "Invalid" << std::endl;
    }
    
    return 0;
}
