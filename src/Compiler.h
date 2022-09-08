#include <string>
#include <iostream>
#include <vector>
#include "Token.h"
#include "Tree.cpp"

class Compiler {
    public:
        void lexer(std::string&);
        void parse(std::vector<Token*>&, int);
        void semanticAnalysis(Tree&, int);


};
