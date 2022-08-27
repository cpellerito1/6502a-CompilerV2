#include "Compiler.h"
#include "Tree.cpp"
#include <algorithm>

// Variable to hold tokenStrem
std::vector<Token*> tokenStream;
// Counter for tokenStream
static int current;
// Variable to hold error state, parse doesn't care how many errors just if there are so use bool
bool isErrors;
// CST
Tree cst;
// Help turn charLists to strings
std::string charToString = "";

Token::Grammar statements[] = {Token::Grammar::KEYWORD,
            Token::Grammar::TYPE, Token::Grammar::ID, Token::Grammar::L_BRACE};
Token::Grammar expressions[] = {Token::Grammar::DIGIT, Token::Grammar::L_QUOTE,
            Token::Grammar::L_PARAN, Token::Grammar::ID, Token::Grammar::BOOL_VAL};
//Token::Grammar abs[] = {Token::Grammar::ID, Token::Grammar::DIGIT,Token::Grammar::TYPE, Token::Grammar::BOOL_VAL};


// Blueprints`
void parseBlock(void);
void parseStateList(void);
void parseState(void);
void parseVarDecl(void);
void parseAssign(void);
void parsePrint(void);
void parseExpr(void);
void parseBoolExpr(void);
void parseStringExpr(void);
void parseCharList(void);
void parseIntExpr(void);
void parseWhile(void);
void parseIf(void);
void match(Token::Grammar);
void matchString(std::string);

/**
 * @brief 
 * 
 * @param tokens 
 * @param programCounter 
 */
void Compiler::parse(std::vector<Token*> &tokens, int programCounter) {
    // Set the global variables and reset the counters from previous programs
    tokenStream = tokens;
    current = 0;
    isErrors = false;

    // Add root node
    cst.addNode("Root", Tree::Kind::ROOT);


    std::cout << "Beginning parse for program " << programCounter << std::endl;
    std::cout << "parse()\n";
    parseBlock();
    match(Token::Grammar::EOP);
    if (!isErrors) {
        std::cout << "Parse finished successfully...\nPrinting CST...\n";
        // Print the CST
        std::cout << cst.toString();
    }



}


void parseBlock() {
    // Add block node
    cst.addNode("Block", Tree::Kind::BRANCH);

    std::cout << "parseBlock()\n";
    match(Token::Grammar::L_BRACE);
    parseStateList();
    match(Token::Grammar::R_BRACE);
    cst.moveUp();

}

void parseStateList() {
    // Add statement list node
    cst.addNode("Statement List", Tree::Kind::BRANCH);
    
    std::cout << "parseStateList()\n";
    if (std::find(std::begin(statements), std::end(statements), tokenStream.at(current)->type) != std::end(statements)) {
        parseState();
        parseStateList();
    }

    cst.moveUp();
}

void parseState() {
    // Add statement node
    cst.addNode("Statement", Tree::Kind::BRANCH);

    std::cout << "parseState()\n";

    Token* t = tokenStream.at(current);
    if (t->value == "print")
        parsePrint();
    else if (t->value == "while")
        parseWhile();
    else if (t->value == "if")
        parseIf();
    else if (t->type == Token::Grammar::ID)
        parseAssign();
    else if (t->type == Token::Grammar::TYPE)
        parseVarDecl();
    else
        parseBlock();
    
    cst.moveUp();
}

void parseVarDecl() {
    // Add variable declaration node
    cst.addNode("Variable Declaration", Tree::Kind::BRANCH);

    std::cout << "parseVarDecl()\n";
    cst.addNode("Type", Tree::Kind::BRANCH);
    match(Token::Grammar::TYPE);
    cst.addNode("ID", Tree::Kind::BRANCH);
    match(Token::Grammar::ID);

    cst.moveUp();
}

void parseAssign() {
    // Add assign node
    cst.addNode("Assignment Statement", Tree::Kind::BRANCH);

    std::cout << "parseAssign()\n";
    cst.addNode("ID", Tree::Kind::BRANCH);
    match(Token::Grammar::ID);
    match(Token::Grammar::ASSIGN_OP);
    parseExpr();

    cst.moveUp();
}

void parsePrint() {
    // Add print node
    cst.addNode("Print Statement", Tree::Kind::BRANCH);

    std::cout << "parsePrint()\n";
    matchString("print");
    match(Token::Grammar::L_PARAN);
    if (std::find(std::begin(expressions), std::end(expressions), tokenStream.at(current)->type) != std::end(expressions))
        parseExpr();
    match(Token::Grammar::R_PARAN);

    cst.moveUp();
}

void parseExpr() {
    // Add expresion node
    cst.addNode("Expression", Tree::Kind::BRANCH);

    std::cout << "parseExpr()\n";
    Token *t = tokenStream.at(current);
    switch (t->type) {
        case Token::Grammar::DIGIT:
            parseIntExpr();
            break;
        case Token::Grammar::L_QUOTE:
            parseStringExpr();
            break;
        case Token::Grammar::L_PARAN:
            parseBoolExpr();
            break;
        case Token::Grammar::BOOL_VAL:
            parseBoolExpr();
            break;
        default:
            cst.addNode("ID", Tree::Kind::BRANCH);
            match(Token::Grammar::ID);
    }

    cst.moveUp();
}

void parseBoolExpr() {
    // Add Bool expr node
    cst.addNode("Boolean Expression", Tree::Kind::BRANCH);

    std::cout << "parseBoolExpr()\n";
    if (tokenStream.at(current)->type == Token::Grammar::L_PARAN) {
        match(Token::Grammar::L_PARAN);
        parseExpr();
        if (tokenStream.at(current)->type == Token::Grammar::EQUAL_OP)
            match(Token::Grammar::EQUAL_OP);
        else
            match(Token::Grammar::IN_EQUAL_OP);

        parseExpr();
        match(Token::Grammar::R_PARAN);
    } else {
        cst.addNode("Bool Val", Tree::Kind::BRANCH);
        match(Token::Grammar::BOOL_VAL);
    }

    cst.moveUp();
}

void parseStringExpr() {
    // Add String expr
    cst.addNode("String Expression", Tree::Kind::BRANCH);

    std::cout << "parseStringExpr()\n";
    match(Token::Grammar::L_QUOTE);
    parseCharList();
    match(Token::Grammar::R_QUOTE);

    cst.moveUp();
}

void parseCharList() {
    // Add char list node
    cst.addNode("Character List", Tree::BRANCH);

    std::cout << "parseCharList()\n";
    Token *t = tokenStream.at(current);
    if (t->type == Token::Grammar::CHAR) {
        match(Token::Grammar::CHAR);
        parseCharList();
    } else if (t->type == Token::Grammar::SPACE) {
        match(Token::Grammar::SPACE);
        parseCharList();
    }

    cst.moveUp();
}

void parseIntExpr() {
    // Add int expr node
    cst.addNode("Integer Expression", Tree::Kind::BRANCH);

    std::cout << "parseIntExpr()\n";
    match(Token::Grammar::DIGIT);
    if (tokenStream.at(current)->type == Token::Grammar::ADD_OP) {
        match(Token::Grammar::ADD_OP);
        parseExpr();
    } 

    cst.moveUp();
}

void parseWhile() {
    // Add while node
    cst.addNode("While Statement", Tree::Kind::BRANCH);

    std::cout << "parseWhile()\n";
    matchString("while");
    parseBoolExpr();
    parseBlock();

    cst.moveUp();
}

void parseIf() {
    cst.addNode("If Statement", Tree::Kind::BRANCH);
    
    std::cout << "parseIf()\n";
    matchString("if");
    parseBoolExpr();
    parseBlock();

    cst.moveUp();
}

void match(Token::Grammar expected) {
    Token *t = tokenStream.at(current);
    if (t->type == expected) {
        cst.addNode(t->value, Tree::Kind::LEAF);
        current++;
    } else {
        std::cout << "Error: expected " << Token::grammarToString(expected) << " but got " <<
             Token::grammarToString(t->type) << " at (" << t->line << ":" << t->pos << ")\n";
    }
}




/**
 * @brief This method is the same as the regular match method but uses a different input. This is needed
 * because of the way I handled keywords in Lex. Instead of making a KEYWORD_PRINT ENUM and creating
 * a regex for each keyword, I used just one regex and made the ENUM just KEYWORD and then stored the
 * type of keyword in the attribute field. Using 2 regex (one for keyword and one for keywordExact) seemed
 * easier than using 6 total regexs to differentiate between the 3 different keywords and the additional logic
 * required.
 * 
 * @param expected string representation of expected type of the current token
 */
void matchString(std::string expected) {
    Token *t = tokenStream.at(current);
    if (t->value == expected) {
        cst.addNode(t->value, Tree::Kind::LEAF);
        current++;
    } else {
        std::string k = t->value;
        if (k == "print" || k == "while" || k == "if") {
            std::cout << "Error: expected " << expected << " but got " << 
                t->value << " at (" << t->line << ":" << t->pos << ")\n";
        } else {
            std::cout << "Error: expected " << expected << " but got " << Token::grammarToString(t->type) <<
                " at (" << t->line << ":" << t->pos << '\n';
        }
    
    }
}