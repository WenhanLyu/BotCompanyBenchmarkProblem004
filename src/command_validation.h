#ifndef COMMAND_VALIDATION_H
#define COMMAND_VALIDATION_H

#include <string>
#include <cctype>

/**
 * Input validation functions for M2 account system commands.
 * All validation functions return true if valid, false otherwise.
 */

/**
 * Validate UserID parameter
 * Requirements:
 * - Length: 1-30 characters
 * - Character set: a-zA-Z0-9_ (alphanumeric and underscore only)
 * - Cannot be empty
 */
inline bool validateUserID(const std::string& userID) {
    // Check length
    if (userID.empty() || userID.length() > 30) {
        return false;
    }
    
    // Check character set: only alphanumeric and underscore
    for (char c : userID) {
        if (!std::isalnum(static_cast<unsigned char>(c)) && c != '_') {
            return false;
        }
    }
    
    return true;
}

/**
 * Validate Password parameter
 * Requirements:
 * - Length: 1-30 characters
 * - Character set: a-zA-Z0-9_ (alphanumeric and underscore only)
 * - Cannot be empty
 */
inline bool validatePassword(const std::string& password) {
    // Check length
    if (password.empty() || password.length() > 30) {
        return false;
    }
    
    // Check character set: only alphanumeric and underscore
    for (char c : password) {
        if (!std::isalnum(static_cast<unsigned char>(c)) && c != '_') {
            return false;
        }
    }
    
    return true;
}

/**
 * Validate Username parameter
 * Requirements:
 * - Length: 1-30 characters
 * - Character set: visible ASCII (excluding invisible characters)
 * - Can contain spaces and special characters
 * - Cannot contain tab, newline, or other control characters
 */
inline bool validateUsername(const std::string& username) {
    // Check length
    if (username.empty() || username.length() > 30) {
        return false;
    }
    
    // Check character set: visible ASCII only (printable characters)
    // ASCII printable range: 32 (space) to 126 (~)
    for (char c : username) {
        unsigned char uc = static_cast<unsigned char>(c);
        if (uc < 32 || uc > 126) {
            return false;
        }
    }
    
    return true;
}

/**
 * Validate Privilege parameter
 * Requirements:
 * - Must be exactly one of: '1', '3', or '7'
 * - Must be a single character string
 * - No other values allowed (not even '0')
 */
inline bool validatePrivilege(const std::string& privilege) {
    // Must be single character
    if (privilege.length() != 1) {
        return false;
    }
    
    // Must be exactly '1', '3', or '7'
    char c = privilege[0];
    return (c == '1' || c == '3' || c == '7');
}

#endif // COMMAND_VALIDATION_H
