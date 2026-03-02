#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>

// Account structure
struct Account {
    std::string username;
    std::string password;
    int privilege;
    
    Account() : privilege(0) {}
    Account(const std::string& user, const std::string& pass, int priv)
        : username(user), password(pass), privilege(priv) {}
};

// Account system class - manages accounts and login state
class AccountSystem {
private:
    Account rootAccount;
    Account* currentUser;  // Pointer to current logged-in user (nullptr if not logged in)
    
public:
    AccountSystem() : currentUser(nullptr) {
        // Initialize root account: username: root, password: sjtu, privilege: 7
        rootAccount = Account("root", "sjtu", 7);
    }
    
    // Login with username and optional password
    // Returns true if successful, false otherwise
    bool login(const std::string& username, const std::string& password = "") {
        // For M1, only support root login
        if (username == "root") {
            if (password == "sjtu") {
                currentUser = &rootAccount;
                return true;
            }
        }
        return false;
    }
    
    // Logout current user
    // Returns true if successful, false if no one is logged in
    bool logout() {
        if (currentUser == nullptr) {
            return false;
        }
        currentUser = nullptr;
        return true;
    }
    
    // Get current privilege level (0 if not logged in)
    int getCurrentPrivilege() const {
        return currentUser ? currentUser->privilege : 0;
    }
    
    // Check if someone is logged in
    bool isLoggedIn() const {
        return currentUser != nullptr;
    }
};

#endif // ACCOUNT_H
