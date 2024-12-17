#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <string>
#include "scanner.h"
#include "parser.h"
#include "staticSemantics.h"
#include "codeGeneration.h"

FILE* filePointer = nullptr;
char* file;
FILE* input = nullptr;

void compile(std::ifstream&);

int main(int argc, char *argv[]) {

    if (argc != 2) {
        const char baseFileName[] = "a";
        file = new char[sizeof(baseFileName)];
        std::strcpy(file, baseFileName);

        char fileName[1024];
        snprintf(fileName, sizeof(fileName), "%s.4280fs24", baseFileName);
        // Read from standard input and write to a temporary file
        std::ofstream tempFile(fileName);
        if (!tempFile) {
            std::cerr << "Error: Could not create temporary file." << std::endl;
            return 1;
        }

        std::cout << "Enter your input (Ctrl+D to finish):" << std::endl;
        std::string line;
        while (std::getline(std::cin, line)) {
            tempFile << line << std::endl;
        }

        tempFile.close();

        // Reopen the temporary file as an ifstream
        std::ifstream inputFile(fileName);
        if (!inputFile) {
            std::cerr << "Error: Could not reopen temporary file." << std::endl;
            return 1;
        }
        compile(inputFile);
    } else {
        // Open the input file
        file = argv[1];
        char fileName[1024];
        snprintf(fileName, sizeof(fileName), "%s.4280fs24", argv[1]);
        std::ifstream inputFile(fileName);
        if (!inputFile) {
            std::cerr << "Error: Could not open file " << argv[1] << std::endl;
            return 1;
        }
        compile(inputFile);
    }

    

    return 0;
}

void compile(std::ifstream &inputFile) {
    // Initialize the scanner with the input file
    Scanner scanner(inputFile);

    try {
        // Parse the program
        Parser parser(scanner);
        auto parseTree = parser.parse();
        if (!parseTree) {
        std::cerr << "ERROR: Parse tree is null!\n";
        return;
        }

        // Perform static semantics checks
        semanticWrapper(parseTree);

        // generate Code
        codeGenerationWrapper(parseTree);
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return;
    }
}

