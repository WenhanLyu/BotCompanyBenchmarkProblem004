#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <string>
#include <vector>
#include <sstream>

/**
 * Parameter extraction utilities for command parsing.
 * These functions help parse command arguments from input lines.
 */

/**
 * Parse command and arguments from a line.
 * Arguments are split by whitespace.
 * 
 * @param line The input line
 * @param command Output: the command name (first word)
 * @param args Output: vector of arguments (remaining words)
 */
inline void parseCommand(const std::string& line, std::string& command, std::vector<std::string>& args) {
    args.clear();
    std::stringstream ss(line);
    ss >> command;
    
    std::string arg;
    while (ss >> arg) {
        args.push_back(arg);
    }
}

/**
 * Parse command with support for a final argument that may contain spaces.
 * This is useful for commands like register and useradd where the username
 * parameter can contain spaces.
 * 
 * @param line The input line
 * @param command Output: the command name (first word)
 * @param args Output: vector of arguments
 * @param fixedArgsCount Number of space-separated arguments before the final argument
 */
inline void parseCommandWithSpacedFinalArg(const std::string& line, 
                                           std::string& command, 
                                           std::vector<std::string>& args,
                                           int fixedArgsCount) {
    args.clear();
    std::stringstream ss(line);
    ss >> command;
    
    // Read fixed arguments (space-separated)
    for (int i = 0; i < fixedArgsCount; ++i) {
        std::string arg;
        if (ss >> arg) {
            args.push_back(arg);
        } else {
            // Not enough arguments
            return;
        }
    }
    
    // Read rest of line as final argument (may contain spaces)
    std::string finalArg;
    std::getline(ss, finalArg);
    
    // Trim leading space from final argument
    if (!finalArg.empty() && finalArg[0] == ' ') {
        finalArg = finalArg.substr(1);
    }
    
    if (!finalArg.empty()) {
        args.push_back(finalArg);
    }
}

/**
 * Extract exactly N space-separated arguments from a line after the command.
 * 
 * @param line The input line
 * @param command Output: the command name (first word)
 * @param args Output: vector of arguments
 * @param count Expected number of arguments
 * @return true if exactly count arguments were extracted, false otherwise
 */
inline bool extractNArgs(const std::string& line, 
                        std::string& command, 
                        std::vector<std::string>& args, 
                        int count) {
    args.clear();
    std::stringstream ss(line);
    ss >> command;
    
    for (int i = 0; i < count; ++i) {
        std::string arg;
        if (ss >> arg) {
            args.push_back(arg);
        } else {
            return false;
        }
    }
    
    // Check that there are no extra arguments
    std::string extra;
    if (ss >> extra) {
        return false;  // Too many arguments
    }
    
    return true;
}

/**
 * Trim leading and trailing whitespace from a string.
 * 
 * @param str The string to trim
 * @return Trimmed string
 */
inline std::string trimWhitespace(const std::string& str) {
    if (str.empty()) {
        return str;
    }
    
    size_t start = 0;
    size_t end = str.length();
    
    // Find first non-whitespace character
    while (start < end && std::isspace(static_cast<unsigned char>(str[start]))) {
        ++start;
    }
    
    // Find last non-whitespace character
    while (end > start && std::isspace(static_cast<unsigned char>(str[end - 1]))) {
        --end;
    }
    
    return str.substr(start, end - start);
}

#endif // COMMAND_PARSER_H
