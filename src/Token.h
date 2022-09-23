#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <iostream>

class Token {
    public:
        enum Grammar {
            TYPE=0, L_BRACE, R_BRACE, ID, CHAR, BOOL_VAL, ADD_OP, EQUAL_OP, ASSIGN_OP,
            IN_EQUAL_OP, ERROR, EOP, DIGIT, WARNING, SPACE, L_QUOTE, R_QUOTE, R_PARAN, L_PARAN,
            KEYWORD, STRING
        };

        // Attributes
        int line;
        int pos;
        std::string value;
        Grammar type;

        // Constructor
        Token(int line, int pos, std::string value, Grammar type) {
            this->line = line;
            this->pos = pos;
            this->value = value;
            this->type = type;
        }

        // Methods
        void toString(void);
        static std::string grammarToString(Token::Grammar);
};

#endif //TOKEN_H