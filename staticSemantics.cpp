#include "staticSemantics.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <vector>
#include <algorithm>

std::vector<std::string> varTable;
std::string usedVars[100];
bool usedVar = false;


void semanticWrapper(const std::shared_ptr<ParseTree>& node) {

    for (int i = 0; i < 100; i++) {
        usedVars[i] = "0";
    }

    staticSemantics(node, 0);

    for (int i = 0; i < varTable.size(); i++) {
        for (int j = 0; j < 100; j++) {
            if (varTable[i] == usedVars[j]) {
                    usedVar = true;
            }
        }

            if (usedVar == false) {
                std::cout << "WARNING: Variable '" << varTable[i] << "' declared but not used.\n";
            } else {
                usedVar = false;
        }
    }
}

void staticSemantics(const std::shared_ptr<ParseTree>& node, int depth = 0) {
    if (!node) return;
    if (node->label == "<VarList>") {
        insert(node);
    } else {
        if (node->token.tokenID == ID_tk) {
            bool inSymbolTable = false;
            inSymbolTable = verify(node);
            if(!inSymbolTable) {
                std::cout << "ERROR - " << node->token.tokenInstance << " is not a declared identifier - Line: " << node->token.lineNumber << "\n";
                exit(EXIT_FAILURE); 
            }
            for(int i = 0; i < 100; i++) {
                if (usedVars[i] == "0") {
                    usedVars[i] = node->token.tokenInstance;
                    break;
             }
            }
        }
    }

    for (const auto& child : node->children) {
        staticSemantics(child, depth + 1);
    }
}

void insert(const std::shared_ptr<ParseTree>& node) {
    bool isNewVar = true;
    if (varTable.empty()) {
        varTable.push_back(node->token.tokenInstance);
        
    } else {
        for (int i = 0; i < varTable.size(); i++) {
            if (varTable[i] == node->token.tokenInstance) {
                isNewVar = false;
                std::cout << "ERROR: " << node->token.tokenInstance << " declared multiple times on line: " << node->token.lineNumber << "\n";
            } 
        }

        if (isNewVar == true) {
            std::cout << "New Variable Pushed to Static Semantics\n------------------------------------------\n\n";
            varTable.push_back(node->token.tokenInstance);
        }
    }


}

bool verify(const std::shared_ptr<ParseTree>& node) {
    for (int i = 0; i < varTable.size(); i++) {
        if (varTable[i] == node->token.tokenInstance) {
            return true;
        }
    }

    return false;
}