// scanner.cpp
#include "scanner.h"
#include <cctype>
#include <iostream>
#include <sstream>

// Initialize the keywords and operators
Scanner::Scanner(std::istream &input) 
    : input(input), 
      keywords({"start", "stop", "iterate", "var", "exit", "read", "print", 
                "iff", "then", "set", "func", "program"}),
      operators({"=", ".le.", ".ge.", ".lt.", ".gt.", "~", ":", ";", "+", 
                 "-", "**", "/", "%", "(", ")", ",", "{", "}", "[", "]"}) {
    readTokens();
}

void Scanner::readTokens() {
    std::string line;
    while (std::getline(input, line)) {
        // Increment line number after reading the line
        lineNumber++; 

        std::istringstream lineStream(line);
        std::string data;

        while (lineStream >> data) {
            // Check for comments and skip them
            if (data == "@@") {
                std::string comment;
                std::getline(lineStream, comment, '@'); // Read until the end of comment
                continue; // Skip processing the comment
            }

            // Handle keywords and identifiers
            if (std::isalpha(data[0])) {
                if (keywords.count(data)) {
                    tokens.push_back({KW_tk, data, lineNumber});
                } else {
                    if (data.length() > 8) {
                        std::cerr << "LEXICAL ERROR: Identifier '" << data << "' exceeds length at line " << lineNumber << std::endl;
                        tokens.push_back({ERROR_tk, data, lineNumber});
                    } else {
                        tokens.push_back({ID_tk, data, lineNumber});
                    }
                }
            } else if (std::isdigit(data[0])) {
                // Handle integers
                if (data.length() > 8) {
                    std::cerr << "LEXICAL ERROR: Integer '" << data << "' exceeds length at line " << lineNumber << std::endl;
                    tokens.push_back({ERROR_tk, data, lineNumber});
                } else {
                    tokens.push_back({INT_tk, data, lineNumber});
                }
            } else if (operators.count(data)) {
                // Handle operators
                tokens.push_back({OP_tk, data, lineNumber});
            } else {
                // Handle lexical error for invalid characters
                std::cerr << "LEXICAL ERROR: Invalid token '" << data << "' at line " << lineNumber << std::endl;
                tokens.push_back({ERROR_tk, data, lineNumber});
            }
        }
    }
    tokens.push_back({EOF_tk, "", lineNumber}); // End of file token
}


Token Scanner::scanner() {
    if (currentTokenIndex < tokens.size()) {
        return tokens[currentTokenIndex++];
    }
    return {EOF_tk, "", lineNumber}; // Return EOF token if no more tokens are available
}
