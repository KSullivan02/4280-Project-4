#ifndef SCANNER_H
#define SCANNER_H

#include <iostream>
#include <vector>
#include <unordered_set>
#include "token.h"

class Scanner{
private:
    std::istream &input;
    std::vector<Token> tokens;
    size_t currentTokenIndex = 0;
    int lineNumber = 0;

    // Reserved Keywords
    const std::unordered_set<std::string> keywords;
    // Reserved Operators
    const std::unordered_set<std::string> operators;

    void readTokens();
public:
    Scanner(std::istream &input);

    Token scanner();

};


#endif