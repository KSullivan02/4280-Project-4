#ifndef CODEGENERATION_H
#define CODEGENERATION_H

#include "parser.h"
#include "staticSemantics.h"

typedef enum {VAR, LABEL} nameType;

void codeGenerationWrapper(const std::shared_ptr<ParseTree>&);
void codeGeneration(const std::shared_ptr<ParseTree>&, int);

void generateExpression(const std::shared_ptr<ParseTree>&);

void generateM(const std::shared_ptr<ParseTree>&);
void generateN(const std::shared_ptr<ParseTree>&);
void generateR(const std::shared_ptr<ParseTree>&);
void generateCond(const std::shared_ptr<ParseTree>&);
void generateIter(const std::shared_ptr<ParseTree>&);


static std::string newName(nameType);

#endif