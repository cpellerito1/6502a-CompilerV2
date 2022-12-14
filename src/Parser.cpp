#include "Compiler.h"
#include <algorithm>

// Variable to hold tokenStrem
std::vector<Token*> tokenStream;
// Counter for tokenStream
static int current;
// Variable to hold error state, parse doesn't care how many errors just if there are so use bool
bool isErrors;
// AST
static Tree ast;
// Help turn charLists to strings
std::string charToString = "";

Token::Grammar statements[] = {Token::Grammar::KEYWORD,
            Token::Grammar::TYPE, Token::Grammar::ID, Token::Grammar::L_BRACE};
Token::Grammar expressions[] = {Token::Grammar::DIGIT, Token::Grammar::L_QUOTE,
            Token::Grammar::L_PARAN, Token::Grammar::ID, Token::Grammar::BOOL_VAL};
Token::Grammar absTree[] = {Token::Grammar::ID, Token::Grammar::DIGIT,Token::Grammar::TYPE, Token::Grammar::BOOL_VAL};


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
void Compiler::parse(std::vector<Token*> &tokens, int &programCounter) {
    // Set the global variables and reset the counters from previous programs
    tokenStream = tokens;
    current = 0;
    isErrors = false;
    ast.clear();

    std::cout << "Beginning parse for program " << programCounter << std::endl;
    std::cout << "parse()\n";
    parseBlock();
    match(Token::Grammar::EOP);
    if (!isErrors) {
        std::cout << "Parse finished successfully...\n\nPrinting AST...\n";
        std::cout << ast.toString();
        puts("");
        Compiler::semanticAnalysis(ast, programCounter);
    } else
        std::cout << "Parse for program " << programCounter << " failed due to error(s)\n";

    // Move to next program if possible
    if (!isFinished())
        std::cout << "Beginning Lex for program " << ++programCounter << '\n';
    
    return;
}


void parseBlock() {
    // Add block node
    ast.addNode("Block", Tree::Kind::BRANCH);

    std::cout << "parseBlock()\n";
    match(Token::Grammar::L_BRACE);
    parseStateList();
    match(Token::Grammar::R_BRACE);
    ast.moveUp();

}

void parseStateList() {    
    std::cout << "parseStateList()\n";

    if (std::find(std::begin(statements), std::end(statements), tokenStream.at(current)->type) != std::end(statements)) {
        parseState();
        parseStateList();
    }
}

void parseState() {
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
}

void parseVarDecl() {
    ast.addNode("Variable Declaration", Tree::Kind::BRANCH);

    std::cout << "parseVarDecl()\n";
    match(Token::Grammar::TYPE);
    match(Token::Grammar::ID);

    ast.moveUp();
}

void parseAssign() {
    ast.addNode("Assignment Statement", Tree::Kind::BRANCH);
    std::cout << "parseAssign()\n";

    match(Token::Grammar::ID);
    match(Token::Grammar::ASSIGN_OP);
    parseExpr();

    ast.moveUp();
}

void parsePrint() {
    ast.addNode("Print Statement", Tree::Kind::BRANCH);

    std::cout << "parsePrint()\n";
    matchString("print");
    match(Token::Grammar::L_PARAN);
    if (std::find(std::begin(expressions), std::end(expressions), tokenStream.at(current)->type) != std::end(expressions))
        parseExpr();
    match(Token::Grammar::R_PARAN);

    ast.moveUp();
}

void parseExpr() {
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
            match(Token::Grammar::ID);
    }
}

void parseBoolExpr() {
    std::cout << "parseBoolExpr()\n";

    if (tokenStream.at(current)->type == Token::Grammar::L_PARAN) {
        match(Token::Grammar::L_PARAN);
        parseExpr();
        if (tokenStream.at(current)->type == Token::Grammar::EQUAL_OP) {
            match(Token::Grammar::EQUAL_OP);
            ast.addNode("Equal", Tree::Kind::BRANCH);
            ast.restructure();
        }
        else {
            match(Token::Grammar::IN_EQUAL_OP);
            ast.addNode("Not Equal", Tree::Kind::BRANCH);
            ast.restructure();
        }
        parseExpr();
        match(Token::Grammar::R_PARAN);
        ast.moveUp();
    } else {
        match(Token::Grammar::BOOL_VAL);
    }
}

void parseStringExpr() {
    std::cout << "parseStringExpr()\n";

    match(Token::Grammar::L_QUOTE);
    parseCharList();
    match(Token::Grammar::R_QUOTE);
}

void parseCharList() {
    std::cout << "parseCharList()\n";

    Token *t = tokenStream.at(current);
    if (t->type == Token::Grammar::CHAR) {
        match(Token::Grammar::CHAR);
        parseCharList();
    } else if (t->type == Token::Grammar::SPACE) {
        match(Token::Grammar::SPACE);
        parseCharList();
    }
}

void parseIntExpr() {
    std::cout << "parseIntExpr()\n";

    match(Token::Grammar::DIGIT);
    if (tokenStream.at(current)->type == Token::Grammar::ADD_OP) {
        match(Token::Grammar::ADD_OP);
        ast.addNode("Add", Tree::Kind::BRANCH);
        ast.restructure();
        parseExpr();
        ast.moveUp();
    } 

}

void parseWhile() {
    ast.addNode("While Statement", Tree::Kind::BRANCH);

    std::cout << "parseWhile()\n";
    matchString("while");
    parseBoolExpr();
    parseBlock();

    ast.moveUp();
}

void parseIf() {
    ast.addNode("If Statement", Tree::Kind::BRANCH);

    std::cout << "parseIf()\n";
    matchString("if");
    parseBoolExpr();
    parseBlock();

    ast.moveUp();
}

void match(Token::Grammar expected) {
    Token *t = tokenStream.at(current);
    if (t->type == expected) {
        current++;
    } else {
        std::cout << "Error: expected " << Token::grammarToString(expected) << " but got " <<
             Token::grammarToString(t->type) << " at (" << t->line << ":" << t->pos << ")\n";
    }

    // Check if the token needs to be turned into a string or added to the ast
    if (t->type == Token::Grammar::CHAR || t->type == Token::Grammar::SPACE) {
        charToString += t->value;
    } else if (t->type == Token::Grammar::R_QUOTE) {
        ast.addNode(charToString, Tree::Kind::LEAF,
            new Token(t->line, t->pos - charToString.size(), charToString, Token::Grammar::STRING));
        // Reset charToString
        charToString = "";
    } else if (std::find(std::begin(absTree), std::end(absTree), t->type) != std::end(absTree)) {
        ast.addNode(t->value, Tree::Kind::LEAF, t);
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
        current++;
    } else {
        // Check if the value of the current token is one of the keywords for correct error output
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