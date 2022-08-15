#include <string>
#include "Token.cpp"

class Compiler {
    public:
        void lexer(std::string*);

        void start(std::string* inputString) {
            lexer(inputString);
        }

};
