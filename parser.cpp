#include "parser.h"

Parser::Parser(Scanner& scanner) : scanner(scanner) {
    advance();
}

void Parser::advance() {
    currentToken = scanner.scanner();
}

void Parser::match(TokenID expected, const std::string& expectedInstance) {
    if (currentToken.tokenID == expected && 
        (expectedInstance.empty() || currentToken.tokenInstance == expectedInstance)) {
        advance();
    } else {
        std::cerr << "SYNTAX ERROR: Expected '" 
                  << (expectedInstance.empty() ? std::to_string(expected) : expectedInstance) 
                  << "' but got '" << currentToken.tokenInstance 
                  << "' at line " << currentToken.lineNumber << std::endl;
        exit(EXIT_FAILURE);
    }
}


std::shared_ptr<ParseTree> Parser::parse() {
    auto root = program();    
    
    advance();
    if (currentToken.tokenID != EOF_tk) {
    std::cerr << "SYNTAX ERROR: Unexpected Input After Program Completion at line " << currentToken.lineNumber << std::endl;
    exit(EXIT_FAILURE);
    }
    
    return root;
    
}



std::shared_ptr<ParseTree> Parser::program() {
    auto node = std::make_shared<ParseTree>("<program>");
    node->token = currentToken;
    match(KW_tk, "program");
    node->children.push_back(vars());
    node->children.push_back(block());
    
    return node;
}

std::shared_ptr<ParseTree> Parser::vars() {
    auto node = std::make_shared<ParseTree>("<vars>");
    node->token = currentToken;
    if (currentToken.tokenInstance == "var") {
        match(KW_tk, "var");
        node->children.push_back(varList());
    }
    // If there's no "var", <vars> is empty (epsilon production).
    return node;
}


std::shared_ptr<ParseTree> Parser::varList() {
    auto node = std::make_shared<ParseTree>("<VarList>");
    node->token = currentToken;
    // Match and store the identifier
    std::string identifier = currentToken.tokenInstance;
    match(ID_tk);
    node->children.push_back(std::make_shared<ParseTree>(identifier));
    
    // Match the comma
    match(OP_tk, ",");

    // Match and store the integer
    std::string integer = currentToken.tokenInstance;
    match(INT_tk);
    node->children.push_back(std::make_shared<ParseTree>(integer));

    // Check if there's another variable declaration
    if (currentToken.tokenID == ID_tk) {
        // Recursively parse the next variable declaration
        node->children.push_back(varList());
    } else {
        // Match the semicolon at the end of the last declaration
        match(OP_tk, ";");
    }

    return node;
}


std::shared_ptr<ParseTree> Parser::block() {
    auto node = std::make_shared<ParseTree>("<block>");
    node->token = currentToken;
    match(KW_tk, "start");
    node->children.push_back(vars());
    node->children.push_back(stats());
    match(KW_tk, "stop");
    return node;
}

std::shared_ptr<ParseTree> Parser::stats() {
    auto node = std::make_shared<ParseTree>("<stats>");
    node->token = currentToken;
    node->children.push_back(stat());
    node->children.push_back(mStat());
    return node;
}

std::shared_ptr<ParseTree> Parser::mStat() {
    auto node = std::make_shared<ParseTree>("<mStat>");
    node->token = currentToken;
    if (currentToken.tokenInstance == "read" || 
        currentToken.tokenInstance == "print" || 
        currentToken.tokenInstance == "start" || 
        currentToken.tokenInstance == "iff" || 
        currentToken.tokenInstance == "iterate" || 
        currentToken.tokenInstance == "set") {
        node->children.push_back(stat());
        node->children.push_back(mStat());
    }
    return node; 
}

std::shared_ptr<ParseTree> Parser::stat() {
    auto node = std::make_shared<ParseTree>("<stat>");
    node->token = currentToken;
    if (currentToken.tokenInstance == "read") {
        node->children.push_back(read());
    } else if (currentToken.tokenInstance == "print") {
        node->children.push_back(print());
    } else if (currentToken.tokenInstance == "start") {
        node->children.push_back(block());
    } else if (currentToken.tokenInstance == "iff") {
        node->children.push_back(cond());
    } else if (currentToken.tokenInstance == "iterate") {
        node->children.push_back(iter());
    } else if (currentToken.tokenInstance == "set") {
        node->children.push_back(assign());
    } else {
        std::cerr << "SYNTAX ERROR: Unexpected STAT token '" 
                  << currentToken.tokenInstance 
                  << "' at line " << currentToken.lineNumber << std::endl;
        exit(EXIT_FAILURE);
    }
    return node;
}

std::shared_ptr<ParseTree> Parser::read() {
    auto node = std::make_shared<ParseTree>("<read>");
    node->token = currentToken;
    match(KW_tk, "read");               // Match the "read" keyword
    auto idNode = std::make_shared<ParseTree>("identifier");
    idNode->token = currentToken;       // Store the current identifier token
    match(ID_tk);                       // Match the identifier
    node->children.push_back(idNode);   // Add identifier as a child of <read>
    match(OP_tk, ";");                  // Match the semicolon
    return node;
}

std::shared_ptr<ParseTree> Parser::print() {
    auto node = std::make_shared<ParseTree>("<print>");
    node->token = currentToken;
    match(KW_tk, "print");              // Match the "print" keyword
    node->children.push_back(exp());    // Parse and add the <exp> subtree
    match(OP_tk, ";");                  // Match the semicolon
    return node;
}

std::shared_ptr<ParseTree> Parser::cond() {
    auto node = std::make_shared<ParseTree>("<cond>");
    node->token = currentToken;
    match(KW_tk, "iff");  // Match the 'iff' keyword
    match(OP_tk, "[");     // Match the opening bracket for the condition

    // Parse the expression inside the condition
    node->children.push_back(exp());

    // Parse the relational operator
    node->children.push_back(relational());

    // Parse the second expression of the condition
    node->children.push_back(exp());

    match(OP_tk, "]");  // Match the closing bracket

    // Parse the statement following the condition
    node->children.push_back(stat());

    return node;
}





std::shared_ptr<ParseTree> Parser::iter() {
    auto node = std::make_shared<ParseTree>("<iter>");
    node->token = currentToken;
    match(KW_tk, "iterate");           // Match the "iterate" keyword
    match(OP_tk, "[");                 // Match the opening square bracket '['

    // Parse and add the first <exp> subtree
    node->children.push_back(exp());

    // Parse and add the <relational> subtree
    node->children.push_back(relational());

    // Parse and add the second <exp> subtree
    node->children.push_back(exp());

    match(OP_tk, "]");                 // Match the closing square bracket ']'

    // Parse and add the <stat> subtree
    node->children.push_back(stat());

    return node;
}

std::shared_ptr<ParseTree> Parser::assign() {
    auto node = std::make_shared<ParseTree>("<assign>");
    node->token = currentToken;
    match(KW_tk, "set");               // Match the "set" keyword

    // Parse and add the identifier
    auto idNode = std::make_shared<ParseTree>("identifier");
    idNode->token = currentToken;      // Capture the current identifier token
    match(ID_tk);                      // Match the identifier
    node->children.push_back(idNode);

    // Parse and add the <exp> subtree
    node->children.push_back(exp());

    match(OP_tk, ";");                 // Match the semicolon

    return node;
}


std::shared_ptr<ParseTree> Parser::exp() {
    auto node = std::make_shared<ParseTree>("<exp>");
    node->children.push_back(M());  // First M (multiplicative expression)

    // Now handle addition or subtraction
    while (currentToken.tokenInstance == "+" || currentToken.tokenInstance == "-") {
        auto opNode = std::make_shared<ParseTree>("operator " + currentToken.tokenInstance);
        opNode->token = currentToken;  // Capture the token for this operator
        node->children.push_back(opNode);  // Add operator to node
        match(OP_tk, currentToken.tokenInstance);  // Match the operator
        auto rhsNode = exp();
        node->children.push_back(rhsNode);
    }

    return node;
}


std::shared_ptr<ParseTree> Parser::M() {
    auto node = std::make_shared<ParseTree>("<M>");
    node->token = currentToken;

    // Parse the first operand (N) from <M>
    node->children.push_back(N());

    // Handle the modulus operator
    while (currentToken.tokenInstance == "%") {
        std::string op = currentToken.tokenInstance;
        match(OP_tk, op);

        auto operatorNode = std::make_shared<ParseTree>(op);
        node->children.push_back(operatorNode);

        // Parse the next operand (N) after the operator
        node->children.push_back(N());
    }

    return node;
}



std::shared_ptr<ParseTree> Parser::N() {
    // Create a root node for <N>
    auto node = std::make_shared<ParseTree>("<N>");

    // Check if the current token is a unary minus
    if (currentToken.tokenInstance == "-") {
        // Handle unary minus operator
        auto operatorNode = std::make_shared<ParseTree>("-");
        match(OP_tk, "-"); // Consume the '-' operator

        // Add the unary minus as a child of the current <N>
        node->children.push_back(operatorNode);

        // Recursively parse the next <N> (the operand of the unary minus)
        node->children.push_back(N()); // The next <N> will be the operand
    } else {
        // No unary minus, just parse the first <R> (operand)
        node->children.push_back(R());
    }

    // Handle division operator '/' if needed
    while (currentToken.tokenInstance == "/") {
        auto operatorNode = std::make_shared<ParseTree>("/");
        match(OP_tk, "/"); // Consume the '/' operator
        
        // Create a new <N> for the division and add the left operand
        auto divisionNode = std::make_shared<ParseTree>("<N>");
        divisionNode->children.push_back(node); // Left operand (current <N>)
        divisionNode->children.push_back(operatorNode); // The division operator
        divisionNode->children.push_back(R()); // Right operand (<R>)

        node = divisionNode; // Update to handle next <N> for further divisions
    }

    return node;
}




std::shared_ptr<ParseTree> Parser::R() {
    auto node = std::make_shared<ParseTree>("<R>");
    node->token = currentToken;

    if (currentToken.tokenID == INT_tk) {
        // If the current token is an integer, add it to the tree
        node->children.push_back(std::make_shared<ParseTree>(currentToken.tokenInstance));
        match(INT_tk);
    } else if (currentToken.tokenID == ID_tk) {
        // If the current token is an identifier, add it to the tree
        node->children.push_back(std::make_shared<ParseTree>(currentToken.tokenInstance));
        match(ID_tk);
    } else if (currentToken.tokenInstance == "(") {
        // Handle parentheses (sub-expression)
        match(OP_tk, "(");

        node->children.push_back(exp());
        
        match(OP_tk, ")");
    } else {
        // Error if none of the expected tokens are found
        std::cerr << "SYNTAX ERROR: Unexpected " << currentToken.tokenInstance << " token at line " << currentToken.lineNumber << std::endl;
        throw std::runtime_error("Parsing failed due to syntax errors");
    }

    return node;
}




std::shared_ptr<ParseTree> Parser::relational() {
    auto node = std::make_shared<ParseTree>("<relational>");
    node->token = currentToken;

    // Handle relational operators
    if (currentToken.tokenInstance == ".le." || currentToken.tokenInstance == ".ge." ||
        currentToken.tokenInstance == ".lt." || currentToken.tokenInstance == ".gt." ||
        currentToken.tokenInstance == "**" || currentToken.tokenInstance == "~") {

        auto opNode = std::make_shared<ParseTree>("relational operator");
        opNode->token = currentToken;  // Capture the operator
        node->children.push_back(opNode);  // Add to node
        match(OP_tk, currentToken.tokenInstance);  // Match the relational operator
    } else {
        throw std::runtime_error("SYNTAX ERROR: Expected a valid relational operator at line " + std::to_string(currentToken.lineNumber));
    }

    return node;
}



