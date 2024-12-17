#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum TokenID {
    KW_tk,
    ID_tk,
    INT_tk,
    OP_tk,
    EOF_tk,
    ERROR_tk
};

struct Token {
    TokenID tokenID;
    std::string tokenInstance;
    int lineNumber;
};

#endif