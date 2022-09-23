#include "Compiler.h"
#include <regex>

// Blueprints
bool isBoundry(char);
std::vector<Token*> stringLexer(std::string&);
void trim(std::vector<Token*>&);

// Counters for iterating over the input file
static int current = 0;
int prev = 0;
int line = 1;
int inputSize;

// Program counter
int programCounter = 0;

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

void Compiler::lexer(std::string &inputString) {
    // Set inputSize variable
    inputSize = inputString.size();
    std::cout << "size: " << inputSize << std::endl;
    std::cout << "Beginning Lex for program " << programCounter << std::endl;
    // Vector to hold the token stream
    std::vector<Token*> tokenStream;

    // Error counter
    int errors = 0;

    // String to store substrings of the inputString
    std::string subString = "";

    // Loop over the char array
    while (current < inputString.size()) {
        // Make sure the current char is in the alphabet of the grammar
        if (!regex_match(inputString.substr(current, 1), all) && !isBoundry(inputString[current])) {
            std::string s(1, inputString[current]);
            tokenStream.push_back(new Token(line, current/line, "unrecognized token \"" + s + "\"", Token::Grammar::ERROR));
            // Increment counters and continue                
            errors++;
            current++;
            prev = current;
            continue;
        }

        // After checking that the char is in the alphabet, we can begin lexical analysis
        if (isBoundry(inputString[current])) {
            // Find out which boundry it is
            if (inputString[current] == '{') {
                // Add the token to the token stream
                tokenStream.push_back(new Token(line, current/line, "{", Token::Grammar::L_BRACE));
                // Iterator current and set prev = current to get the next char then continue
                current++;
                prev = current;
                continue;
            } else if (inputString[current] == '}') {
                tokenStream.push_back(new Token(line, current/line, "}", Token::Grammar::R_BRACE));
                current++;
                prev = current;
                continue;
            } else if (inputString[current] == '=') {
                // Check if next char is an equal sign also to differentiate assignent and equality ops
                if (inputString[current + 1] == '=') {
                    tokenStream.push_back(new Token(line, current/line, "==", Token::Grammar::EQUAL_OP));
                    current += 2;
                } else {
                    tokenStream.push_back(new Token(line, current/line, "=", Token::Grammar::ASSIGN_OP));
                    current++;
                }
                prev = current;
                continue;
            } else if (inputString[current] == '!') {
                if (inputString[current + 1] == '=') {
                    tokenStream.push_back(new Token(line, current/line, "!=", Token::Grammar::IN_EQUAL_OP));
                    current += 2;
                } else {
                    tokenStream.push_back(new Token(line, current/line, "unrecognized token '!'", Token::Grammar::ERROR));
                    errors++;
                    current++;
                }
                prev = current;
                continue;
            } else if (inputString[current] == '/') {
                if (inputString[current + 1] == '*') {
                    // Create a temp token now with the start info of the comment so that if
                    // the comment is unterminated at end of file this token will provide its starting point
                    tempToken = new Token(line, current/line, "unclosed comment at end of file", Token::Grammar::WARNING);
                    current += 2;
                    // Now find the end of the comment
                    while (current < inputString.size()) {
                        if (inputString[current] == '*') {
                            if (current + 1 < inputString.size() && inputString[current] == '/') {
                                current += 2;
                                break;
                            }

                        } else 
                            current++;
                        
                    }
                    // After the while loop check to see if current is larger than size of inputString
                    // If it is, that means the comment was unclosed so add the tempToken to the tokenStream
                    // If it isn't, that means the comment was closed and we can continue
                    if (current >= inputString.size()) {
                        tokenStream.push_back(tempToken);
                        tempToken = NULL;
                    }
                    prev = current;
                    continue;

                } else {
                    // If the next character is not *, then "/" is an invalid token
                    tokenStream.push_back(new Token(line, current/line, "unrecognized token /", Token::Grammar::ERROR));
                    errors++;
                    current++;
                    prev = current;
                    continue;
                }
            } else if (inputString[current] == '\n') {
                line++;
                current++;
                prev = current;
                continue;
            } else if (inputString[current] == '+') {
                tokenStream.push_back(new Token(line, current/line, "+", Token::Grammar::ADD_OP));
                current++;
                prev = current;
                continue;
            } else if (inputString[current] == '\"') {
                tokenStream.push_back(new Token(line, current/line, "\"", Token::Grammar::L_QUOTE));
                current++;
                prev = current;

                // Fill temp token for unclosed quote warning
                tempToken = new Token(line, current/line, "unclosed quote at end of file", Token::Grammar::WARNING);
                // Temp indexCounter counter to keep line positioning correct
                int indexCounter = current;
                // Loop through the inputString to find end of quote
                while (indexCounter < inputString.size()) {
                    if (inputString[indexCounter] == '\"')
                        break;
                    else
                        indexCounter++;
                }
                // Again check if indexCounter is greater than the size of the inputString
                if (indexCounter >= inputString.size()) {
                    tokenStream.push_back(tempToken);
                    tempToken = NULL;
                } else {
                    // Quote was valid and terminated so call stringLexer and add the tokens to the tokenStream
                    std::string s = inputString.substr(current, indexCounter - current);
                    std::vector<Token*> stringTokens = stringLexer(s);
                    tokenStream.insert(tokenStream.end(), stringTokens.begin(), stringTokens.end());
                    // Add closing quote
                    tokenStream.push_back(new Token(line, current/line, "\"", Token::Grammar::R_QUOTE));
                }
                current = indexCounter + 1;
                prev = current;
                continue;
            } else if (inputString[current] == '(') {
                tokenStream.push_back(new Token(line, current/line, "(", Token::Grammar::L_PARAN));
                current++;
                prev = current;
                continue;
            } else if (inputString[current] == ')') {
                tokenStream.push_back(new Token(line, current/line, ")", Token::Grammar::R_PARAN));
                current++;
                prev = current;
                continue;
            } else if (inputString[current] == '$') {
                tokenStream.push_back(new Token(line, current/line, "$", Token::Grammar::EOP));
                current++;
                prev = current;
                if (errors == 0) {
                    for (auto t: tokenStream) {
                        t->toString();
                    }
                    // Remove warnings
                    trim(tokenStream);
                    // Print new line before parse
                    puts("");
                    // Begin parse
                    parse(tokenStream, programCounter);

                } else {
                    std::cout << "Skipping parse due to (" << errors << ") lex errors\n" << std::endl;
                }

                // Clear tokenStream for the next program
                tokenStream.clear();
                continue;
            } else {
                // The current character is a white space
                if (current + 1 < inputString.size()) {
                    current++;
                    prev++;
                    continue;
                } else {
                    subString = inputString.substr(prev, current - prev);
                }
            }
        } else {
            // Current character is not a boudry so add it to substring for regex matching
            subString = inputString.substr(prev, (current - prev) + 1);
        }

        // Regex matching
        // Keyword matcher (if, print, while)
        if (regex_match(subString, keyword)) {
            while (!regex_match(subString, keywordExact)) {
                std::string temp(1, subString[0]);
                // If it isnt an exact match, the chars in front of it must be IDs
                tokenStream.push_back(new Token(line, prev/line, temp, Token::Grammar::ID));
                // Remove the char from the string
                subString.erase(0, 1);
                prev++;
            }

            // Add the keyword token
            tokenStream.push_back(new Token(line, prev/line, subString, Token::Grammar::KEYWORD));
            current++;
            prev = current;
        } else if (regex_match(subString, type)) {
            while (!regex_match(subString, typeExact)) {
                std::string temp(1, subString[0]);
                // If it isnt an exact match, the chars in front of it must be IDs
                tokenStream.push_back(new Token(line, prev/line, temp, Token::Grammar::ID));
                // Remove the char from the string
                subString.erase(0, 1);
                prev++;
            }

            tokenStream.push_back(new Token(line, prev/line, subString, Token::Grammar::TYPE));
            current++;
            prev = current;
        } else if (regex_match(subString, boolean)) {
            while (!regex_match(subString, boolExact)){
                std::string temp(1, subString[0]);
                // If it isnt an exact match, the chars in front of it must be IDs
                tokenStream.push_back(new Token(line, prev/line, temp, Token::Grammar::ID));
                // Remove the char from the string
                subString.erase(0, 1);
                prev++;
            }

            tokenStream.push_back(new Token(line, prev/line, subString, Token::Grammar::BOOL_VAL));
            current++;
            prev = current;
        } else if (regex_match(subString, character)) {
            if (current + 1 < inputString.size()) {
                if (isBoundry(inputString[current + 1]) || regex_match(inputString.substr(current + 1, 1), digit)) {
                    // Next char is a boundry or number, this must be an id. Use a for loop
                    // to create the tokens for other chars that may be in the subString
                    for (int i = 0; i < subString.size(); i++, prev++) {
                        std::string temp(1, subString[i]); 
                        tokenStream.push_back(new Token(line, prev/line, temp, Token::Grammar::ID));
                    }

                    current++;
                    prev = current;
                } else {
                    // If not a boundry or digit, increment and move to next char
                    current++;
                }
            } else {
                tokenStream.push_back(new Token(line, prev/line, subString, Token::Grammar::ID));
                current++;
            }
        } else if (regex_match(subString, digit)) {
            tokenStream.push_back(new Token(line, prev/line, subString, Token::Grammar::DIGIT));
            current++;
            prev = current;
        } else {
            current++;
        }
    }

    // Make sure tokenStream is empty and the last token in there is an EOP. If not throw a warning and add the
    // EOP assuming the user meant to put one as this is the end of the file
    if (!tokenStream.empty() && tokenStream.back()->type == Token::Grammar::EOP) {
        tokenStream.push_back(new Token(line, current/line, 
                    "file ended with no end of program symbol($), EOP inserted here", Token::Grammar::WARNING));
        tokenStream.push_back(new Token(line, (current+1)/line, "$", Token::Grammar::EOP));

        for (auto t: tokenStream) {
            t->toString();
        }

        if (errors == 0) {
            // Remove warnings from tokenStream so parse doesnt have to deal with them
            trim(tokenStream);

            // Begin parse
            parse(tokenStream, programCounter);
        } else {
            std::cout << "Skipping parse for program " << programCounter << " due to (" << errors << ") lex errors\n";
        }
    }
}


/**
 * @brief Checks if a character is a boundry
 * 
 * @param input char from inputArray
 * @return true if is boundry
 * @return false if not boudry
 */
bool isBoundry(char input) {
    char operators[] = {'=', '!', '\n', ' ', '{', '}', '$', '/', '\"', '+', '(', ')'};
    for (char op: operators) {
        if (op == input)
            return true;
    }
    return false;
}


/**
 * @brief This is a convience function. It performs lexical anlaysis on a string. The string will be parsed
 * in the main lexer function and then passed to here. This helps reduce nested logic so the main lexing
 * does not have to worry about if every character it finds is inside a string
 * 
 * @param input string reference to the string parsed from the inputArray
 * @return std::vector<Token*>* pointer to the vector containing the tokens to be added to tokenStream
 */
std::vector<Token*> stringLexer(std::string &input) {
    std::vector<Token*> temp;

    for (int i = 0; i < input.size(); i++) {
        if (regex_match(input.substr(i, 1), character)) {
            std::string s(1, input[i]);
            temp.push_back(new Token(line, current/line, s, Token::Grammar::CHAR));
        } else if (input[i] == ' ')
            temp.push_back(new Token(line, current/line, " ", Token::Grammar::SPACE));
        else if (input[i] == '\n') {
            temp.push_back(new Token(line, current/line, "string can't have new line character", Token::Grammar::ERROR));
            line++;
        } else if (input[i] == '$') {
            temp.push_back(new Token(line, current/line, "possible unclosed string", Token::Grammar::WARNING));
            temp.push_back(new Token(line, current/line, "unrecognized token $ in string", Token::Grammar::ERROR));
        } else {
            std::string s(1, input[i]);
            temp.push_back(new Token(line, current/line, "unexpected token \"" + s + "\"", Token::Grammar::ERROR));
        }

        // Keep current = i so line positioning is correct
        current++;
    }

    return temp;
}


/**
 * @brief This function removes the warnings from the tokenStream. This is done when there are warnings
 * but no errors since the tokens will be be passed to parse. This makes it so parse does not have to
 * deal with warning tokens
 * 
 * @param tokenStream 
 */
void trim(std::vector<Token*> &tokenStream) {
     for (int i = 0; i < tokenStream.size(); i++) {
        if (tokenStream[i]->type == Token::Grammar::WARNING)
            tokenStream.erase(tokenStream.begin() + (i-1));
    }

    tokenStream.shrink_to_fit();
}


bool Compiler::isFinished() {
    return current + 1 >= inputSize;
}

