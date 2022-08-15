#include <iostream>
#include <string>
#include <map>

class Token {
    public:
        enum Grammar {
        TYPE, L_BRACE, R_BRACE, ID, CHAR, BOOL_VAL, ADD_OP, EQUAL_OP, ASSIGN_OP,
        IN_EQUAL_OP, ERROR, EOP, DIGIT, WARNING, SPACE, L_QUOTE, R_QUOTE, R_PARAN, L_PARAN,
        KEYWORD, STRING
    };
        // Attributes
        int *line;
        int pos;
        std::string value;
        Grammar type;

        // Constructor
        Token(int *line, int pos, std::string value, Grammar type) {
            line = line;
            pos = pos;
            value = value;
            type = type;
        }

        // Methods
        std::string grammarToString(Grammar type) {
            switch (type) {
                case L_BRACE:
                    return "L_BRACE";
                case R_BRACE:
                    return "R_BRACE";
                case ID:
                    return "ID";
                default:
                    return "Error";
            }
        }

        std::string toString() {
            if (type == ERROR) {
                return {"Error: " + value + " at " + (char)*line + ":" + (char)pos + '\n'};
            } else if (type == WARNING) {
                return {"Warning: " + value + " at " + (char)*line + ":" + (char)pos + '\n'};
            } else {
                return {"VERBOSE Lexer - " + grammarToString(type) + "[ " + value + " ] found at (" +
                                (char)*line + ":" + (char)pos + ")\n"};
            }
        }
};