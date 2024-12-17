#include "codeGeneration.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

std::vector<std::string> tempVarTable;


static int LabelCntr = 0; /* counting unique labels generated */
static int VarCntr = 0; /* counting unique temporaries generated */


static std::vector<std::string> names; // Vector to store names

void codeGenerationWrapper(const std::shared_ptr<ParseTree>& node) {
    std::cout << "Starting of Code Generation Wrapper\n------------------------------------------------\n\n";

    char fileBuf[100];
    sprintf(fileBuf, "%s.asm", file);

    filePointer = fopen(fileBuf, "w");

    std::cout << "Starting Code Generation\n------------------------------------------\n\n";
    codeGeneration(node, 0);

    fprintf(filePointer, "STOP\n");

    // Initialize all variables as zero
    if (!varTable.empty()) {
        for(size_t i = 0; i < varTable.size(); i++) {
          fprintf(filePointer, "%s 0\n", varTable[i].c_str());
         }
    }

    //Initialize all temp variables as zero
    if (!tempVarTable.empty()) {
        for (size_t i = 0; i < tempVarTable.size(); i++) {
            fprintf(filePointer, "%s 0\n", tempVarTable[i].c_str());
        }
    }

}

void codeGeneration(const std::shared_ptr<ParseTree>& node, int depth = 0) {

    if (!node) return;

    else if (node->label == "<program>") {
        for (const auto& child : node->children) {
            codeGeneration(child, depth + 1); // Continue processing children
        }
    }

    else if (node->label == "<print>") {
        std::cout << "Beginning Print Generation\n------------------------------------------------\n\n";
        codeGeneration(node->children[0]);
        std::string aVar = newName(VAR);

        fprintf(filePointer, "STORE %s\n", aVar.c_str());
        fprintf(filePointer, "WRITE %s\n", aVar.c_str());
    }

    else if (node->label == "<read>") {
        std::cout << "Beginning Read Generation\n------------------------------------------------\n\n";
        fprintf(filePointer, "READ %s\n", node->children[0]->token.tokenInstance.c_str());
    }

    else if (node->label == "<assign>") {
        std::cout << "Beginning Assign Generation\n------------------------------------------------\n\n";
        codeGeneration(node->children[1]);
        fprintf(filePointer, "STORE %s\n", node->children[0]->token.tokenInstance.c_str());
    }

    else if (node->label == "<exp>") {
        generateExpression(node);
    }

    else if (node->label == "<M>") {
        generateM(node);
    }

    else if (node->label == "<N>") {
        generateN(node);
    }

    else if (node->label == "<R>") {
        generateR(node);
    }

    else if (node->label == "<cond>") {
        generateCond(node);
    }

    else if (node->label == "<iter>") {
        generateIter(node);
    }

    else {
        for (const auto& child : node->children) {
            codeGeneration(child, depth + 1);
        }
    }
}

// exp
void generateExpression(const std::shared_ptr<ParseTree>& node) {

    std::cout << "Beginning Expression Generation <expr> is " << node->token.tokenInstance << "\n------------------------------------------------\n\n";
    std::cout << "Node Type: " << node->label << "\n";

    if (node->children.empty()) {
        std::cout << "somehow this shit empty\n";
    }

    if (node->children.size() == 1) {
        codeGeneration(node->children[0]);
    } else {
        // Call furthest right node
        codeGeneration(node->children.back());

        // Generate Temp
        std::string aVar = newName(VAR);
        fprintf(filePointer, "STORE %s\n", aVar.c_str());
        codeGeneration(node->children[0]);
        if (node->children[1]->token.tokenInstance == "+") {
            fprintf(filePointer, "ADD %s\n", aVar.c_str());
        } 
        if (node->children[1]->token.tokenInstance == "-") {
            std::cout << "SUB ACTIVATED\n";
            fprintf(filePointer, "SUB %s\n", aVar.c_str());
        }
    }
}


void generateM(const std::shared_ptr<ParseTree>& node) {
    std::cout << "Beginning M Generation\n------------------------------------------------\n\n";
    if (node->children.size() == 1) {
        codeGeneration(node->children[0]);
    } else {
        std::cout << "REACHED MODULO\n";

        // Run Far Right Child
        codeGeneration(node->children.back());

        // store in tempVar
        std::string aVar = newName(VAR);
        fprintf(filePointer, "STORE %s\n", aVar.c_str());

        // Run far left Child
        codeGeneration(node->children[0]);
        std::string bVar = newName(VAR);
        fprintf(filePointer, "STORE %s\n", bVar.c_str());

        // Now that both are stored, run Modulo
        fprintf(filePointer, "DIV %s\n", aVar.c_str());
        fprintf(filePointer, "MULT %s\n", aVar.c_str());
        fprintf(filePointer, "STORE %s\n", aVar.c_str());

        fprintf(filePointer, "LOAD %s\n", bVar.c_str());

        fprintf(filePointer, "SUB %s\n", aVar.c_str());
    }
    
}

void generateN(const std::shared_ptr<ParseTree>& node) {
    std::cout << "Beginning N Generation\n------------------------------------------------\n\n";
    if (node->children.size() == 1) {
        codeGeneration(node->children[0]);
    } else {
        if (node->children.size() == 3) {
            std::cout << "REACHED DIVISION\n";
            codeGeneration(node->children.back());
            std::string aVar = newName(VAR);
            fprintf(filePointer, "STORE %s\n", aVar.c_str());

            codeGeneration(node->children[0]);
            fprintf(filePointer, "DIV %s\n", aVar.c_str());
        } else {
            std::cout << "UNARY REACHED\n";
            codeGeneration(node->children.back());

            std::string bVar = newName(VAR);
            fprintf(filePointer, "STORE %s\n" , bVar.c_str());
            fprintf(filePointer, "LOAD 0\n");
            fprintf(filePointer, "SUB 1\n");
            fprintf(filePointer, "MULT %s\n", bVar.c_str());
        }
    }
}

void generateR(const std::shared_ptr<ParseTree>& node) {
    std::cout << "Beginning R Generation\n------------------------------------------------\n\n";
    if (node->children[0]->label == "<exp>") {
        codeGeneration(node->children[0]);
    } else if (node->children.size() == 1) {
        fprintf(filePointer, "LOAD %s\n", node->token.tokenInstance.c_str());
    }
}

// Conditional If no Else
void generateCond(const std::shared_ptr<ParseTree>& node) {
    std::cout << "Beginning Cond Generation\n------------------------------------------------\n\n"; 
    codeGeneration(node->children[2]);

    std::string aVar = newName(VAR);
    fprintf(filePointer, "STORE %s\n", aVar.c_str());

    // Grab left side, subtract right side and compare appropriately 
    codeGeneration(node->children[0]);
    fprintf(filePointer, "SUB %s\n", aVar.c_str());

    std::string bLabel = newName(LABEL);
    
    // Greater Than
    if (node->children[1]->token.tokenInstance == ".gt.") {
        
        fprintf(filePointer, "BRZNEG %s\n", bLabel.c_str());
        codeGeneration(node->children.back());
        std::cout << "GENERATED STATEMENT\n";
        fprintf(filePointer, "%s: NOOP\n", bLabel.c_str()); 
        std::cout << "IF FALSE GENERATED\n";
    }

    // Lesser Than
    if (node->children[1]->token.tokenInstance == ".lt.") {
        
        fprintf(filePointer, "BRZPOS %s\n", bLabel.c_str());
        codeGeneration(node->children.back());
        std::cout << "GENERATED STATEMENT\n";
        fprintf(filePointer, "%s: NOOP\n", bLabel.c_str()); 
        std::cout << "IF FALSE GENERATED\n";
    }

    // Greater Equal
    if (node->children[1]->token.tokenInstance == ".ge.") {
        
        fprintf(filePointer, "BRPOS %s\n", bLabel.c_str());
        codeGeneration(node->children.back());
        std::cout << "GENERATED STATEMENT\n";
        fprintf(filePointer, "%s: NOOP\n", bLabel.c_str()); 
        std::cout << "IF FALSE GENERATED\n";
    }

    // Lesser Equal
    if (node->children[1]->token.tokenInstance == ".le.") {
        
        fprintf(filePointer, "BRNEG %s\n", bLabel.c_str());
        codeGeneration(node->children.back());
        std::cout << "GENERATED STATEMENT\n";
        fprintf(filePointer, "%s: NOOP\n", bLabel.c_str()); 
        std::cout << "IF FALSE GENERATED\n";
    }

    // Equal
    if (node->children[1]->token.tokenInstance == "**") {
        
        fprintf(filePointer, "BRNEG %s\n", bLabel.c_str());
        fprintf(filePointer, "BRPOS %s\n", bLabel.c_str());
        codeGeneration(node->children.back());
        std::cout << "GENERATED STATEMENT\n";
        fprintf(filePointer, "%s: NOOP\n", bLabel.c_str()); 
        std::cout << "IF FALSE GENERATED\n";
    }

    // Not Equal
    if (node->children[1]->token.tokenInstance == "~") {
        
        fprintf(filePointer, "BRZERO %s\n", bLabel.c_str());
        codeGeneration(node->children.back());
        std::cout << "GENERATED STATEMENT\n";
        fprintf(filePointer, "%s: NOOP\n", bLabel.c_str()); 
        std::cout << "IF FALSE GENERATED\n";
    }
}

void generateIter(const std::shared_ptr<ParseTree>& node) {
    std::cout << "Beginning Iter Generation\n------------------------------------------------\n\n";
    // Generate start of the loop (entry label)
    std::string loopStart = newName(LABEL);
    fprintf(filePointer, "%s: NOOP\n", loopStart.c_str());

    // Generate right-hand side of relational expression
    codeGeneration(node->children[2]); // Right operand
    std::string rightVar = newName(VAR);
    fprintf(filePointer, "STORE %s\n", rightVar.c_str());

    // Generate left-hand side of relational expression and compare
    codeGeneration(node->children[0]); // Left operand
    fprintf(filePointer, "SUB %s\n", rightVar.c_str());

    // Generate exit label
    std::string exitLabel = newName(LABEL);

    // Branching logic based on the relational operator
    const std::string& relOp = node->children[1]->token.tokenInstance;
    if (relOp == ".gt.") { // Greater Than
        fprintf(filePointer, "BRZNEG %s\n", exitLabel.c_str());
    } else if (relOp == ".lt.") { // Less Than
        fprintf(filePointer, "BRZPOS %s\n", exitLabel.c_str());
    } else if (relOp == ".ge.") { // Greater or Equal
        fprintf(filePointer, "BRPOS %s\n", exitLabel.c_str());
    } else if (relOp == ".le.") { // Less or Equal
        fprintf(filePointer, "BRNEG %s\n", exitLabel.c_str());
    } else if (relOp == "**") { // Equal
        fprintf(filePointer, "BRNEG %s\n", exitLabel.c_str());
        fprintf(filePointer, "BRPOS %s\n", exitLabel.c_str());
    } else if (relOp == "~") { // Not Equal
        fprintf(filePointer, "BRZERO %s\n", exitLabel.c_str());
    }

    // Generate loop body
    codeGeneration(node->children.back());

    // Branch back to the start of the loop
    fprintf(filePointer, "BR %s\n", loopStart.c_str());

    // Exit label
    fprintf(filePointer, "%s: NOOP\n", exitLabel.c_str());
}


static std::string newName(nameType what)
{
    std::string newName;

    if (what == VAR) {
        // Creating a new temporary variable
        newName = "t" + std::to_string(VarCntr++); // Generate a new label as t0, t1, etc.
    } else {
        // Creating a new label
        newName = "L" + std::to_string(LabelCntr++); // New labels as L0, L1, etc.
    }

    // Store the new name in the vector
    tempVarTable.push_back(newName);
    std::cout << "Successfully Created Temp Variable\n------------------------------------------------\n\n";
    return newName;
}