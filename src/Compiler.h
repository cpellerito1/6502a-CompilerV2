#ifndef COMPILER_H
#define COMPILER_H

#include <string>
#include <iostream>
#include <vector>
#include "Token.h"
#include "Tree.cpp"
#include "SymbolTable.cpp"

class Compiler {
    public:
        void lexer(std::string&);
        bool isFinished(void);
        void parse(std::vector<Token*>&, int&);
        void semanticAnalysis(Tree&, int&);
        void codeGen(Tree&, int&);


};

#endif // COMPILER_H
