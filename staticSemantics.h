#ifndef STATICSEMANTICS_H
#define STATICSEMANTICS_H

#include "parser.h"

extern std::vector<std::string> varTable;
extern FILE* filePointer;
extern char* file;

void staticSemantics(const std::shared_ptr<ParseTree>&, int);

void insert(const std::shared_ptr<ParseTree>&);

void semanticWrapper(const std::shared_ptr<ParseTree>&);

bool verify(const std::shared_ptr<ParseTree>&);

#endif