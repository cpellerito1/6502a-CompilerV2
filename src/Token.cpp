#include "Compiler.h"

static const char *enumStrings[] = {"TYPE", "L_BRACE", "R_BRACE", "ID", "CHAR", "BOOL_VAL", "ADD_OP",
         "EQUAL_OP", "ASSIGN_OP", "IN_EQUAL_OP", "ERROR", "EOP", "DIGIT", "WARNING", "SPACE", "L_QUOTE",
          "R_QUOTE", "R_PARAN", "L_PARAN", "KEYWORD", "STRING"};

void Token::toString() {
            if (type == Grammar::ERROR) {
                std::cout<< "Error: " << value << " at " << line << ":" << pos << '\n';
            } else if (type == Grammar::WARNING) {
                std::cout << "Warning: " << value << " at " << line << ":" << pos << '\n';
            } else {
                std::cout << "VERBOSE Lexer - " << enumStrings[type] << " [ " << value << " ] found at (" << line << ":" << pos << ")\n";
            }
        }