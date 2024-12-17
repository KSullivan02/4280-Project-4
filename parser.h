#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include "token.h"
#include <memory>
#include <iostream>
#include <string>




struct ParseTree {
    std::string label;
    std::vector<std::shared_ptr<ParseTree>> children;
    Token token;

    ParseTree(const std::string& lbl) : label(lbl) {}
};



class Parser {
private:
    Scanner& scanner;
    Token currentToken;

    void advance();
    void match(TokenID expected, const std::string& expectedInstance = "");


    std::shared_ptr<ParseTree> program();
    std::shared_ptr<ParseTree> vars();
    std::shared_ptr<ParseTree> varList();
    std::shared_ptr<ParseTree> stats();
    std::shared_ptr<ParseTree> mStat();
    std::shared_ptr<ParseTree> stat();
    std::shared_ptr<ParseTree> block();
    std::shared_ptr<ParseTree> read();
    std::shared_ptr<ParseTree> print();
    std::shared_ptr<ParseTree> cond();
    std::shared_ptr<ParseTree> iter();
    std::shared_ptr<ParseTree> assign();
    std::shared_ptr<ParseTree> exp();
    std::shared_ptr<ParseTree> M();
    std::shared_ptr<ParseTree> N();
    std::shared_ptr<ParseTree> R();
    std::shared_ptr<ParseTree> relational();

public:
    Parser(Scanner& scanner);
    std::shared_ptr<ParseTree> parse();
};

#endif
