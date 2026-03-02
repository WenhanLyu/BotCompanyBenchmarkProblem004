#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>
#include <map>
#include <stack>
#include <fstream>
#include <sstream>

// Account structure - stored in file and memory
struct Account {
    std::string userID;      // 1-30 chars, digits/letters/underscore
    std::string password;    // 1-30 chars, digits/letters/underscore
    std::string username;    // 1-30 chars, visible ASCII
    int privilege;           // 0, 1, 3, or 7
    bool isDeleted;          // Tombstone flag
    
    Account() : privilege(0), isDeleted(false) {}
    Account(const std::string& uid, const std::string& pass, 
            const std::string& uname, int priv)
        : userID(uid), password(pass), username(uname), 
          privilege(priv), isDeleted(false) {}
};

// Login session - one entry in the login stack
struct LoginSession {
    std::string userID;
    int privilege;
    std::string selectedBook;  // ISBN or empty
    
    LoginSession(const std::string& uid, int priv)
        : userID(uid), privilege(priv), selectedBook("") {}
};

// Account system class - manages accounts, login stack, and file persistence
class AccountSystem {
private:
    static const std::string ACCOUNTS_FILE;
    
    std::map<std::string, Account> accounts;  // userID -> Account (in-memory)
    std::stack<LoginSession> loginStack;      // Login stack
    
    // File I/O operations
    void loadAccounts();
    void saveAccount(const Account& account);
    void updateAccount(const Account& account);
    void deleteAccount(const std::string& userID);
    
    // Helper functions
    bool isValidUserIDOrPassword(const std::string& str) const;
    bool isValidUsername(const std::string& str) const;
    bool isValidPrivilege(int priv) const;
    bool accountExists(const std::string& userID) const;
    bool isAccountLoggedIn(const std::string& userID) const;
    
public:
    AccountSystem();
    
    // Login/logout operations
    bool su(const std::string& userID, const std::string& password);
    bool logout();
    
    // Account management commands
    bool registerAccount(const std::string& userID, const std::string& password, 
                        const std::string& username);
    bool useradd(const std::string& userID, const std::string& password, 
                int privilege, const std::string& username);
    bool passwd(const std::string& userID, const std::string& currentPassword,
               const std::string& newPassword, bool hasCurrentPassword);
    bool deleteUser(const std::string& userID);
    
    // Privilege and state queries
    int getCurrentPrivilege() const;
    int getAccountPrivilege(const std::string& userID) const;
    bool isLoggedIn() const;
    std::string getCurrentSelectedBook() const;
    void setSelectedBook(const std::string& isbn);
    
    // Reporting
    void reportEmployee() const;  // Outputs employee report to stdout
};

#endif // ACCOUNT_H
