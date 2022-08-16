#include "Compiler.h"
#include <iostream>
#include <regex>
#include <vector>

// Blueprints
bool isBoundry(std::string*);
std::vector<Token*> stringLexer(std::string*);

// Counters for iterating over the input file
int current = 0;
int prev = 0;
int line = 1;

// Program counter
int counter = 0;

// Regular expression definitions
std::regex type("int|string|boolean");
std::regex typeExact("^int$|^string$|^boolean$");
std::regex character("[a-z]");
std::regex boolean{"true|false"};
std::regex boolExact{"^true$|^false$"};
std::regex digit{"[0-9]"};
std::regex keyword{"while|if|print"};
std::regex keywordExact{"^while$|^if$|^print$"};
std::regex all{"[a-z|0-9]"};

// Temp token pointer to help with warnings
Token *tempToken;

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
            // Find out which boundry it is
            if (inputString[current] == "{") {
                // Add the token to the token stream
                tokenStream.push_back(new Token(&line, current/line, "{", Token::Grammar::L_BRACE));
                // Iterator current and set prev = current to get the next char then continue\
                current++;
                prev = current;
                continue;
            } else if (inputString[current] == "}") {
                tokenStream.push_back(new Token(&line, current/line, "}", Token::Grammar::R_BRACE));
                current++;
                prev = current;
                continue;
            } else if (inputString[current] == "=") {
                // Check if next char is an equal sign also to differentiate assignent and equality ops
                if (inputString[current + 1] == "=") {
                    tokenStream.push_back(new Token(&line, current/line, "==", Token::Grammar::EQUAL_OP));
                    current += 2;
                } else {
                    tokenStream.push_back(new Token(&line, current/line, "=", Token::Grammar::ASSIGN_OP));
                    current++;
                }
                prev = current;
                continue;
            } else if (inputString[current] == "!") {
                if (inputString[current + 1] == "=") {
                    tokenStream.push_back(new Token(&line, current/line, "!=", Token::Grammar::IN_EQUAL_OP));
                    current += 2;
                } else {
                    tokenStream.push_back(new Token(&line, current/line, "unrecognized token '!'", Token::Grammar::ERROR));
                    current++;
                }
                prev = current;
                continue;
            } else if (inputString[current] == "/") {
                if (inputString[current + 1] == "*") {
                    // Create a temp token now with the start info of the comment so that if
                    // the comment is unterminated at end of file this token will provide its starting point
                    tempToken = new Token(&line, current/line, "unclosed comment at end of file", Token::Grammar::WARNING);
                    current += 2;
                    // Now find the end of the comment
                    while (current < inputString->size()) {
                        if (inputString[current] == "*") {
                            if (current + 1 < inputString->size() && inputString[current] == "/") {
                                current += 2;
                                break;
                            }

                        } else 
                            current++;
                        
                    }
                    // After the while loop check to see if current is larger than size of inputString
                    // If it is, that means the comment was unclosed so add the tempToken to the tokenStream
                    // If it isn't, that means the comment was closed and we can continue
                    if (current >= inputString->size()) {
                        tokenStream.push_back(tempToken);
                        tempToken = NULL;
                    }
                    prev = current;
                    continue;

                } else {
                    // If the next character is not *, then "/" is an invalid token
                    tokenStream.push_back(new Token(&line, current/line, "unrecognized token /", Token::Grammar::ERROR));
                    current++;
                    prev = current;
                    continue;
                }
            } else if (inputString[current] == "\n") {
                line++;
                current++;
                prev = current;
                continue;
            } else if (inputString[current] == "+") {
                tokenStream.push_back(new Token(&line, current/line, "+", Token::Grammar::ADD_OP));
                current++;
                prev = current;
                continue;
            } else if (inputString[current] == "\"") {
                tokenStream.push_back(new Token(&line, current/line, "\"", Token::Grammar::L_QUOTE));
                current++;
                prev = current;

                // Fill temp token for unclosed quote warning
                tempToken = new Token(&line, current/line, "unclosed quote at end of file", Token::Grammar::WARNING);
                // Temp counter to keep line positioning correct
                int temp = current;
                // Loop through the inputString to find end of quote
                while (temp < inputString->size()) {
                    if (inputString[temp] == "\"")
                        break;
                    else
                        temp++;
                }
                // Again check if temp is greater than the size of the inputString
                if (temp >= inputString->size()) {
                    tokenStream.push_back(tempToken);
                    tempToken = NULL;
                } else {
                    // Quote was valid and terminated so call stringLexer and add the tokens to the tokenStream
                    std::string s = inputString->substr(temp, current - temp);
                    std::vector<Token*> stringTokens = stringLexer(&s);
                    tokenStream.insert(tokenStream.end(), stringTokens.begin(), stringTokens.end());
                }
                current = temp + 1;
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


std::vector<Token*> stringLexer(std::string *input) {
    std::vector<Token*> temp;

    for (int i = 0; i < input->size(); i++) {
        if (regex_match(input[i], character))
            temp.push_back(new Token(&line, current/line, input[i], Token::Grammar::CHAR));
        else if (input[i] == " ")
            temp.push_back(new Token(&line, current/line, " ", Token::Grammar::SPACE));
        else if (input[i] == "\n") {
            temp.push_back(new Token(&line, current/line, "string can't have new line character", Token::Grammar::ERROR));
            line++;
        } else if (input[i] == "$") {
            temp.push_back(new Token(&line, current/line, "possible unclosed string", Token::Grammar::WARNING));
            temp.push_back(new Token(&line, current/line, "unrecognized token $ in string", Token::Grammar::ERROR));
        } else {
            temp.push_back(new Token(&line, current/line, "unexpected token \"" + input[i] + "\"", Token::Grammar::ERROR));
        }

        // Keep current = i so line positioning is correct
        current++;
    }

    return temp;
}

