#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "account.h"
#include "book.h"

// Parse command and arguments from normalized string
void parseCommand(const std::string& normalized, std::string& command, std::vector<std::string>& args) {
    std::stringstream ss(normalized);
    ss >> command;
    
    std::string arg;
    while (ss >> arg) {
        args.push_back(arg);
    }
}

// Parse command but preserve spaces in last argument (for username)
void parseCommandWithSpaces(const std::string& normalized, std::string& command, std::vector<std::string>& args, int maxArgs) {
    std::stringstream ss(normalized);
    ss >> command;
    
    // Read first (maxArgs - 1) arguments normally
    for (int i = 0; i < maxArgs - 1; ++i) {
        std::string arg;
        if (ss >> arg) {
            args.push_back(arg);
        } else {
            return;
        }
    }
    
    // Read rest of line as last argument (may contain spaces)
    std::string lastArg;
    std::getline(ss, lastArg);
    // Trim leading space
    if (!lastArg.empty() && lastArg[0] == ' ') {
        lastArg = lastArg.substr(1);
    }
    if (!lastArg.empty()) {
        args.push_back(lastArg);
    }
}

int main() {
    std::string line;
    AccountSystem accountSystem;
    BookSystem bookSystem;
    
    // Main command loop - read line by line
    while (std::getline(std::cin, line)) {
        // Trim and normalize whitespace, but preserve structure for username parsing
        // We'll handle whitespace more carefully per command
        
        // Empty line - skip (no output)
        if (line.empty()) {
            continue;
        }
        
        // Extract the command (first word)
        std::stringstream ss(line);
        std::string command;
        ss >> command;
        
        // Handle quit and exit commands
        if (command == "quit" || command == "exit") {
            break;
        }
        
        // Handle su command
        // Syntax: su [UserID] [Password]
        // Password can be omitted if current privilege > target privilege
        if (command == "su") {
            std::string userID, password;
            ss >> userID;
            
            // Read password if provided (might be empty)
            if (ss >> password) {
                // Password was provided
            } else {
                // Password was not provided - set to empty
                password = "";
            }
            
            if (userID.empty()) {
                std::cout << "Invalid" << std::endl;
                continue;
            }
            
            if (accountSystem.su(userID, password)) {
                // Success - no output
            } else {
                std::cout << "Invalid" << std::endl;
            }
            continue;
        }
        
        // Handle logout command
        if (command == "logout") {
            if (accountSystem.logout()) {
                // Success - no output
            } else {
                std::cout << "Invalid" << std::endl;
            }
            continue;
        }
        
        // Handle register command
        // Syntax: register [UserID] [Password] [Username]
        // Username can contain spaces
        if (command == "register") {
            std::string userID, password;
            ss >> userID >> password;
            
            // Read rest of line as username (may contain spaces)
            std::string username;
            std::getline(ss, username);
            // Trim leading space
            if (!username.empty() && username[0] == ' ') {
                username = username.substr(1);
            }
            
            if (userID.empty() || password.empty() || username.empty()) {
                std::cout << "Invalid" << std::endl;
                continue;
            }
            
            if (accountSystem.registerAccount(userID, password, username)) {
                // Success - no output
            } else {
                std::cout << "Invalid" << std::endl;
            }
            continue;
        }
        
        // Handle useradd command
        // Syntax: useradd [UserID] [Password] [Privilege] [Username]
        // Username can contain spaces
        if (command == "useradd") {
            std::string userID, password, privStr;
            ss >> userID >> password >> privStr;
            
            // Read rest of line as username (may contain spaces)
            std::string username;
            std::getline(ss, username);
            // Trim leading space
            if (!username.empty() && username[0] == ' ') {
                username = username.substr(1);
            }
            
            if (userID.empty() || password.empty() || privStr.empty() || username.empty()) {
                std::cout << "Invalid" << std::endl;
                continue;
            }
            
            // Parse privilege (must be single digit)
            if (privStr.length() != 1 || !std::isdigit(privStr[0])) {
                std::cout << "Invalid" << std::endl;
                continue;
            }
            int privilege = privStr[0] - '0';
            
            if (accountSystem.useradd(userID, password, privilege, username)) {
                // Success - no output
            } else {
                std::cout << "Invalid" << std::endl;
            }
            continue;
        }
        
        // Handle passwd command
        // Syntax: passwd [UserID] ([CurrentPassword])? [NewPassword]
        // For privilege {7}: passwd [UserID] [NewPassword] (2 params)
        // For privilege < {7}: passwd [UserID] [CurrentPassword] [NewPassword] (3 params)
        if (command == "passwd") {
            std::vector<std::string> args;
            std::string arg;
            while (ss >> arg) {
                args.push_back(arg);
            }
            
            bool success = false;
            int currentPriv = accountSystem.getCurrentPrivilege();
            
            if (currentPriv == 7 && args.size() == 2) {
                // Privilege {7}: 2 parameters (no current password)
                success = accountSystem.passwd(args[0], "", args[1], false);
            } else if (currentPriv < 7 && args.size() == 3) {
                // Privilege < {7}: 3 parameters (with current password)
                success = accountSystem.passwd(args[0], args[1], args[2], true);
            } else {
                std::cout << "Invalid" << std::endl;
                continue;
            }
            
            if (success) {
                // Success - no output
            } else {
                std::cout << "Invalid" << std::endl;
            }
            continue;
        }
        
        // Handle delete command
        // Syntax: delete [UserID]
        if (command == "delete") {
            std::string userID;
            ss >> userID;
            
            if (userID.empty()) {
                std::cout << "Invalid" << std::endl;
                continue;
            }
            
            if (accountSystem.deleteUser(userID)) {
                // Success - no output
            } else {
                std::cout << "Invalid" << std::endl;
            }
            continue;
        }
        
        // Handle select command
        // Syntax: select [ISBN]
        // Requires privilege >= 1 (user must be logged in)
        // If book exists, select it; if not, create it and select it
        if (command == "select") {
            // Check if user is logged in
            if (!accountSystem.isLoggedIn()) {
                std::cout << "Invalid" << std::endl;
                continue;
            }
            
            // Check privilege (must be >= 1 to select books)
            int currentPriv = accountSystem.getCurrentPrivilege();
            if (currentPriv < 1) {
                std::cout << "Invalid" << std::endl;
                continue;
            }
            
            // Parse ISBN
            std::string isbn;
            ss >> isbn;
            
            if (isbn.empty()) {
                std::cout << "Invalid" << std::endl;
                continue;
            }
            
            // Try to select existing book
            bool bookExists = bookSystem.selectBook(isbn);
            
            if (!bookExists) {
                // Book doesn't exist - create it
                if (!bookSystem.createBook(isbn)) {
                    // Failed to create (invalid ISBN)
                    std::cout << "Invalid" << std::endl;
                    continue;
                }
            }
            
            // Set selected book in login session
            accountSystem.setSelectedBook(isbn);
            
            // Success - no output
            continue;
        }
        
        // Handle modify command
        // Syntax: modify [-ISBN=value] [-name="value"] [-author="value"] [-keyword="value"] [-price=value]
        // Requires privilege >= 3 and a book must be selected
        if (command == "modify") {
            // Check if user is logged in
            if (!accountSystem.isLoggedIn()) {
                std::cout << "Invalid" << std::endl;
                continue;
            }
            
            // Check privilege (must be >= 3 to modify books)
            int currentPriv = accountSystem.getCurrentPrivilege();
            if (currentPriv < 3) {
                std::cout << "Invalid" << std::endl;
                continue;
            }
            
            // Check if a book is selected
            std::string selectedISBN = accountSystem.getCurrentSelectedBook();
            if (selectedISBN.empty()) {
                std::cout << "Invalid" << std::endl;
                continue;
            }
            
            // Parse parameters
            std::string newISBN = "";
            std::string newName = "";
            std::string newAuthor = "";
            std::string newKeyword = "";
            double newPrice = -1.0; // -1 means not specified
            bool parseError = false;
            
            std::string param;
            while (ss >> param) {
                // Check if parameter starts with '-'
                if (param[0] != '-') {
                    parseError = true;
                    break;
                }
                
                // Find the '=' sign
                size_t eqPos = param.find('=');
                if (eqPos == std::string::npos) {
                    parseError = true;
                    break;
                }
                
                std::string paramName = param.substr(1, eqPos - 1);
                std::string paramValue = param.substr(eqPos + 1);
                
                // Remove quotes if present
                if (paramValue.length() >= 2 && paramValue[0] == '"' && paramValue[paramValue.length() - 1] == '"') {
                    paramValue = paramValue.substr(1, paramValue.length() - 2);
                }
                
                // Process parameter
                if (paramName == "ISBN") {
                    if (!newISBN.empty()) {
                        // Duplicate parameter
                        parseError = true;
                        break;
                    }
                    newISBN = paramValue;
                } else if (paramName == "name") {
                    if (!newName.empty()) {
                        parseError = true;
                        break;
                    }
                    newName = paramValue;
                } else if (paramName == "author") {
                    if (!newAuthor.empty()) {
                        parseError = true;
                        break;
                    }
                    newAuthor = paramValue;
                } else if (paramName == "keyword") {
                    if (!newKeyword.empty()) {
                        parseError = true;
                        break;
                    }
                    newKeyword = paramValue;
                } else if (paramName == "price") {
                    if (newPrice >= 0) {
                        parseError = true;
                        break;
                    }
                    try {
                        newPrice = std::stod(paramValue);
                        if (newPrice < 0) {
                            parseError = true;
                            break;
                        }
                    } catch (...) {
                        parseError = true;
                        break;
                    }
                } else {
                    // Unknown parameter
                    parseError = true;
                    break;
                }
            }
            
            if (parseError) {
                std::cout << "Invalid" << std::endl;
                continue;
            }
            
            // Must have at least one parameter
            if (newISBN.empty() && newName.empty() && newAuthor.empty() && 
                newKeyword.empty() && newPrice < 0) {
                std::cout << "Invalid" << std::endl;
                continue;
            }
            
            // Modify the book
            if (bookSystem.modifyBook(selectedISBN, newISBN, newName, newAuthor, newKeyword, newPrice)) {
                // Success - update selected book ISBN if it changed
                if (!newISBN.empty()) {
                    accountSystem.setSelectedBook(newISBN);
                }
                // No output on success
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
