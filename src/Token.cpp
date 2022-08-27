#include "Compiler.h"

// Const char* array to hold string versions of the enums
static const char *enumStrings[] = {"TYPE", "L_BRACE", "R_BRACE", "ID", "CHAR", "BOOL_VAL", "ADD_OP",
         "EQUAL_OP", "ASSIGN_OP", "IN_EQUAL_OP", "ERROR", "EOP", "DIGIT", "WARNING", "SPACE", "L_QUOTE",
          "R_QUOTE", "R_PARAN", "L_PARAN", "KEYWORD", "STRING"};

// Prints string representation of a Token
void Token::toString() {
            if (type == Grammar::ERROR) {
                std::cout<< "Error: " << value << " at " << line << ":" << pos << '\n';
            } else if (type == Grammar::WARNING) {
                std::cout << "Warning: " << value << " at " << line << ":" << pos << '\n';
            } else {
                std::cout << "VERBOSE Lexer - " << enumStrings[type] << " [ " << value << 
                        " ] found at (" << line << ":" << pos << ")\n";
            }
        }

// Used to get the string value of Grammar enum from outside of the token class
std::string Token::grammarToString(Token::Grammar type) {
    return enumStrings[type];
}