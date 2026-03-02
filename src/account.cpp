#include "account.h"
#include <cctype>
#include <algorithm>
#include <vector>
#include <iostream>
#include <iomanip>

const std::string AccountSystem::ACCOUNTS_FILE = "accounts.dat";

AccountSystem::AccountSystem() {
    loadAccounts();
}

// Load accounts from file, create root if file doesn't exist
void AccountSystem::loadAccounts() {
    std::ifstream infile(ACCOUNTS_FILE);
    
    if (!infile.is_open()) {
        // File doesn't exist - create root account
        Account root("root", "sjtu", "System Administrator", 7);
        accounts["root"] = root;
        saveAccount(root);
        return;
    }
    
    // Read accounts from file
    // Format: UserID|Password|Username|Privilege|IsDeleted
    std::string line;
    while (std::getline(infile, line)) {
        if (line.empty()) continue;
        
        std::stringstream ss(line);
        std::string userID, password, username, privStr, deletedStr;
        
        // Parse pipe-delimited format
        if (std::getline(ss, userID, '|') &&
            std::getline(ss, password, '|') &&
            std::getline(ss, username, '|') &&
            std::getline(ss, privStr, '|') &&
            std::getline(ss, deletedStr, '|')) {
            
            int privilege = std::stoi(privStr);
            bool isDeleted = (deletedStr == "1");
            
            Account account(userID, password, username, privilege);
            account.isDeleted = isDeleted;
            
            // Only load non-deleted accounts
            if (!isDeleted) {
                accounts[userID] = account;
            }
        }
    }
    
    infile.close();
    
    // If no root account exists after loading, create one
    if (accounts.find("root") == accounts.end()) {
        Account root("root", "sjtu", "System Administrator", 7);
        accounts["root"] = root;
        saveAccount(root);
    }
}

// Append new account to file
void AccountSystem::saveAccount(const Account& account) {
    std::ofstream outfile(ACCOUNTS_FILE, std::ios::app);
    if (!outfile.is_open()) return;
    
    // Format: UserID|Password|Username|Privilege|IsDeleted
    outfile << account.userID << "|" 
            << account.password << "|" 
            << account.username << "|" 
            << account.privilege << "|" 
            << (account.isDeleted ? "1" : "0") << "\n";
    
    outfile.flush();
    outfile.close();
}

// Update existing account in file (rewrite entire file)
void AccountSystem::updateAccount(const Account& account) {
    // Read all accounts from file
    std::ifstream infile(ACCOUNTS_FILE);
    std::vector<std::string> lines;
    std::string line;
    
    while (std::getline(infile, line)) {
        if (line.empty()) continue;
        
        std::stringstream ss(line);
        std::string userID;
        std::getline(ss, userID, '|');
        
        // If this is the account we're updating, replace it
        if (userID == account.userID) {
            std::stringstream newLine;
            newLine << account.userID << "|" 
                    << account.password << "|" 
                    << account.username << "|" 
                    << account.privilege << "|" 
                    << (account.isDeleted ? "1" : "0");
            lines.push_back(newLine.str());
        } else {
            lines.push_back(line);
        }
    }
    infile.close();
    
    // Write all accounts back to file
    std::ofstream outfile(ACCOUNTS_FILE);
    for (const auto& l : lines) {
        outfile << l << "\n";
    }
    outfile.flush();
    outfile.close();
}

// Mark account as deleted (tombstone approach)
void AccountSystem::deleteAccount(const std::string& userID) {
    if (accounts.find(userID) != accounts.end()) {
        accounts[userID].isDeleted = true;
        updateAccount(accounts[userID]);
        accounts.erase(userID);
    }
}

// Validate UserID or Password: 1-30 chars, only digits/letters/underscore
bool AccountSystem::isValidUserIDOrPassword(const std::string& str) const {
    if (str.empty() || str.length() > 30) return false;
    
    for (char c : str) {
        if (!std::isalnum(c) && c != '_') {
            return false;
        }
    }
    return true;
}

// Validate Username: 1-30 chars, visible ASCII excluding invisible characters
bool AccountSystem::isValidUsername(const std::string& str) const {
    if (str.empty() || str.length() > 30) return false;
    
    for (char c : str) {
        // Check for visible ASCII (33-126) or space (32)
        if (c < 32 || c > 126) {
            return false;
        }
    }
    return true;
}

// Validate privilege: must be 0, 1, 3, or 7
bool AccountSystem::isValidPrivilege(int priv) const {
    return (priv == 0 || priv == 1 || priv == 3 || priv == 7);
}

// Check if account exists in memory
bool AccountSystem::accountExists(const std::string& userID) const {
    return accounts.find(userID) != accounts.end();
}

// Check if account is anywhere in the login stack
bool AccountSystem::isAccountLoggedIn(const std::string& userID) const {
    std::stack<LoginSession> tempStack = loginStack;
    while (!tempStack.empty()) {
        if (tempStack.top().userID == userID) {
            return true;
        }
        tempStack.pop();
    }
    return false;
}

// Get current privilege (top of stack, or 0 if empty)
int AccountSystem::getCurrentPrivilege() const {
    if (loginStack.empty()) return 0;
    return loginStack.top().privilege;
}

// Get privilege of a specific account
int AccountSystem::getAccountPrivilege(const std::string& userID) const {
    if (!accountExists(userID)) return -1;
    return accounts.at(userID).privilege;
}

// Check if someone is logged in
bool AccountSystem::isLoggedIn() const {
    return !loginStack.empty();
}

// Get current selected book
std::string AccountSystem::getCurrentSelectedBook() const {
    if (loginStack.empty()) return "";
    return loginStack.top().selectedBook;
}

// Set selected book for current session
void AccountSystem::setSelectedBook(const std::string& isbn) {
    if (!loginStack.empty()) {
        // We need to modify the top of stack, but stack doesn't allow direct modification
        // So we pop, modify, and push back
        LoginSession current = loginStack.top();
        loginStack.pop();
        current.selectedBook = isbn;
        loginStack.push(current);
    }
}

// Login: push new session onto stack
bool AccountSystem::su(const std::string& userID, const std::string& password) {
    // Check if account exists
    if (!accountExists(userID)) {
        return false;
    }
    
    // Check if password is required
    // Password can be omitted if current privilege > target privilege
    int currentPriv = getCurrentPrivilege();
    int targetPriv = accounts[userID].privilege;
    bool passwordRequired = (currentPriv <= targetPriv);
    
    // If password is required (or provided), verify it
    if (passwordRequired || !password.empty()) {
        if (accounts[userID].password != password) {
            return false;
        }
    }
    
    // Create new login session and push onto stack
    LoginSession session(userID, accounts[userID].privilege);
    loginStack.push(session);
    return true;
}

// Logout: pop from stack
bool AccountSystem::logout() {
    if (loginStack.empty()) {
        return false;
    }
    loginStack.pop();
    return true;
}

// Register: create new account with privilege {1}
bool AccountSystem::registerAccount(const std::string& userID, 
                                    const std::string& password, 
                                    const std::string& username) {
    // Validate parameters
    if (!isValidUserIDOrPassword(userID)) return false;
    if (!isValidUserIDOrPassword(password)) return false;
    if (!isValidUsername(username)) return false;
    
    // Check for duplicate UserID
    if (accountExists(userID)) return false;
    
    // Create account with privilege {1}
    Account newAccount(userID, password, username, 1);
    accounts[userID] = newAccount;
    saveAccount(newAccount);
    
    return true;
}

// Useradd: create new account with specified privilege
bool AccountSystem::useradd(const std::string& userID, 
                           const std::string& password, 
                           int privilege, 
                           const std::string& username) {
    // Check current privilege (must be at least {3})
    int currentPriv = getCurrentPrivilege();
    if (currentPriv < 3) return false;
    
    // Validate parameters
    if (!isValidUserIDOrPassword(userID)) return false;
    if (!isValidUserIDOrPassword(password)) return false;
    if (!isValidUsername(username)) return false;
    if (!isValidPrivilege(privilege)) return false;
    
    // Privilege 0 cannot be created
    if (privilege == 0) return false;
    
    // Check privilege constraint: new_privilege < current_privilege
    if (privilege >= currentPriv) return false;
    
    // Check for duplicate UserID
    if (accountExists(userID)) return false;
    
    // Create account
    Account newAccount(userID, password, username, privilege);
    accounts[userID] = newAccount;
    saveAccount(newAccount);
    
    return true;
}

// Passwd: change password
bool AccountSystem::passwd(const std::string& userID, 
                          const std::string& currentPassword,
                          const std::string& newPassword,
                          bool hasCurrentPassword) {
    // Check current privilege (must be at least {1})
    int currentPriv = getCurrentPrivilege();
    if (currentPriv < 1) return false;
    
    // Check if account exists
    if (!accountExists(userID)) return false;
    
    // Validate new password
    if (!isValidUserIDOrPassword(newPassword)) return false;
    
    // If not privilege {7}, verify current password
    if (currentPriv < 7) {
        if (!hasCurrentPassword) return false;
        if (accounts[userID].password != currentPassword) return false;
    }
    
    // Update password
    accounts[userID].password = newPassword;
    updateAccount(accounts[userID]);
    
    return true;
}

// Delete: remove account
bool AccountSystem::deleteUser(const std::string& userID) {
    // Check current privilege (must be {7})
    int currentPriv = getCurrentPrivilege();
    if (currentPriv != 7) return false;
    
    // Check if account exists
    if (!accountExists(userID)) return false;
    
    // Check if account is logged in anywhere in stack
    if (isAccountLoggedIn(userID)) return false;
    
    // Cannot delete root
    if (userID == "root") return false;
    
    // Delete account
    deleteAccount(userID);
    
    return true;
}

// Generate employee report and output to stdout
void AccountSystem::reportEmployee() const {
    // Count employees by privilege level
    int countRoot = 0;      // privilege 7
    int countEmployee = 0;  // privilege 3
    int countCustomer = 0;  // privilege 1
    int countGuest = 0;     // privilege 0
    
    // Collect all accounts and sort by userID
    std::vector<Account> sortedAccounts;
    for (const auto& pair : accounts) {
        sortedAccounts.push_back(pair.second);
    }
    std::sort(sortedAccounts.begin(), sortedAccounts.end(), 
              [](const Account& a, const Account& b) {
                  return a.userID < b.userID;
              });
    
    // Count by privilege
    for (const auto& account : sortedAccounts) {
        switch (account.privilege) {
            case 7: countRoot++; break;
            case 3: countEmployee++; break;
            case 1: countCustomer++; break;
            case 0: countGuest++; break;
        }
    }
    
    // Output report in readable format
    std::cout << "=== Employee Report ===" << std::endl;
    std::cout << "Total Accounts: " << sortedAccounts.size() << std::endl;
    std::cout << std::endl;
    
    std::cout << "By Privilege Level:" << std::endl;
    std::cout << "  Root (7):      " << countRoot << std::endl;
    std::cout << "  Employee (3):  " << countEmployee << std::endl;
    std::cout << "  Customer (1):  " << countCustomer << std::endl;
    std::cout << "  Guest (0):     " << countGuest << std::endl;
    std::cout << std::endl;
    
    std::cout << "User Accounts:" << std::endl;
    std::cout << std::setw(20) << std::left << "UserID"
              << std::setw(30) << "Username"
              << std::setw(10) << "Privilege" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    
    for (const auto& account : sortedAccounts) {
        std::cout << std::setw(20) << std::left << account.userID
                  << std::setw(30) << account.username
                  << std::setw(10) << account.privilege << std::endl;
    }
}
