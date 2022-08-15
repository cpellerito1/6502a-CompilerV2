#include "Compiler.h"
#include <iostream>
#include <regex>
#include <vector>
#include <cstring>

// Blueprints
bool isBoundry(std::string*);
std::vector<Token> stringLexer(std::string*);

// Counters for iterating over the input file
int current = 0;
int prev = 0;
int line = 1;

// Program counter
int counter = 0;

// Regular expression definitions
std::regex boolean{"true|false"};
std::regex boolExact{"^true$|^false$"};
std::regex digit{"[0-9]"};
std::regex keyword{"while|if|print"};
std::regex keywordExact{"^while$|^if$|^print$"};
std::regex all{"[a-z|0-9]"};

void Compiler::lexer(std::string *inputString) {
    // Vector to hold the token stream
    std::vector<Token*> tokenStream;

    // Error counter
    int errors = 0;

    // Loop over the char array
    while (current < inputString->size()) {
        // Make sure the current char is in the alphabet of the grammar
        if (!regex_match(inputString[current], all) && !isBoundry(&inputString[current])) {
            tokenStream.push_back(new Token(&line, current/line, "unrecognized token \"" +
                            inputString[current] + "\"", Token::Grammar::ERROR));
            // Increment counters and continue                
            errors++;
            current++;
            prev = current;
            continue;
        }

        // After checking that the char is in the alphabet, we can begin lexical analysis
        if (isBoundry(&inputString[current])) {
            std::string boundry = inputString[current];
            // Find out which boundry it is
            if (boundry == "{") {
                // Add the token to the token stream
                tokenStream.push_back(new Token(&line, current/line, boundry, Token::Grammar::L_BRACE));
                // Iterator current and set prev = current to get the next char then continue\
                current++;
                prev = current;
                continue;
            } else if (boundry == "}") {
                tokenStream.push_back(new Token(&line, current/line, boundry, Token::Grammar::R_BRACE));
                current++;
                prev = current;
                continue;
            } else if (boundry == "=") {
                // Check if next char is an equal sign also to differentiate assignent and equality ops
                if (inputString[current + 1] == "=") {
                    tokenStream.push_back(new Token(&line, current/line, std::string(2, '='), Token::Grammar::EQUAL_OP));
                    current += 2;
                } else {
                    tokenStream.push_back(new Token(&line, current/line, boundry, Token::Grammar::ASSIGN_OP));
                    current++;
                }
                prev = current;
                continue;
            }
        }
    }
}



bool isBoundry(std::string *input) { 
    char operators[] = {'=', '!', '\n', ' ', '{', '}', '$', '/', '"', '+', '(', ')'};
    for (char op: operators) {
        std::string opS(1, op);

        if (opS == *input)
            return true;
    }

    return false;
}

