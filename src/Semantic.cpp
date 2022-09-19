#include "Compiler.h"
#include <unordered_map>
#include <vector>

// Global variables
static Tree ast;
int scope;
bool errors;
//Symbol Table
static SymbolTable symbol;

// Blueprints
void traverse(Tree::Node*);
void checkBlock(Tree::Node*);
void checkVarDecl(Tree::Node*);
void checkAssign(Tree::Node*);
void checkWhile(Tree::Node*);
void checkIf(Tree::Node*);
void checkPrint(Tree::Node*);
void checkExpr(Tree::Node*);
void checkAdd(Tree::Node*);
void checkBoolExpr(Tree::Node*);
SymbolTable::Node* findSymbol(Tree::Node*);
std::string getType(Tree::Node*);
std::vector<std::string> printSymbolTable(SymbolTable::Node*);


void Compiler::semanticAnalysis(Tree &t, int &programCounter) {
    std::cout << "Beginning Semantic Analysis for program " << programCounter << '\n';
    ast = t;
    scope = 0;
    errors = false;
    // Traverse the AST starting from root
   traverse(ast.root);

    if (errors == false) {
        // Print symbol table
        std::cout << "Printing Symbol Table...\nID | Type | Scope | Line\n";
        std::vector<std::string> warnings = printSymbolTable(symbol.root);
        for (std::vector<std::string>::iterator itr = warnings.begin(); itr != warnings.end(); itr++)
            std::cout << *itr << '\n';
        std::cout << "Semantic anlaysis for program " << programCounter << " finished successfully\n";
        //codeGen(ast, programCounter);
    } else {
        std::cout << "Semantic anlaysis failed for program " << programCounter << '\n';
    }
}


void traverse(Tree::Node *node) {
    std::string name = node->name;
    if (name == "Block") {
        checkBlock(node);
    } else if (name == "Variable Declaration") {
        checkVarDecl(node);
    } else if (name == "Assignment Statement") {
        checkAssign(node);
    } else if (name == "While Statement") {
        checkWhile(node);
    } else if (name == "If Statement") {
        checkIf(node);
    } else if (name == "Print Statement") {
        checkPrint(node);
    }

    if (!node->children.empty()) {
        for (auto c: node->children)
            traverse(c);
    }

    // Decrement scope after traversing the children of the block
    if (name == "Block")
        scope--;
}

void checkBlock(Tree::Node *node) {
    scope++;
    symbol.addNode();
}

void checkVarDecl(Tree::Node *node) {
    Tree::Node *type = node->children.at(0);
    Tree::Node *id = node->children.at(1);

    // Check if variable is in symbol table in the current scope
    if (symbol.current->st.find(id->name) != symbol.current->st.end()) {
        Token *t = (Token*)id->token;
        std::cout << "Error8: Variable (" << t->value << ") already decalred in this scope on line " << 
            t->line << ":" << t->pos << '\n';
        errors = true;
    } else {
        symbol.current->st[id->name] = new SymbolTable::SymbolNode(type->name, scope, (Token*)id->token);
    }
}

void checkAssign(Tree::Node *node) {
    Tree::Node *id = node->children.at(0);
    Tree::Node *val = node->children.at(1);

    // Check if id is in current or parent scope
    SymbolTable::Node *n = findSymbol(id);
    if (n == nullptr) {
        Token *t = (Token*)id->token;
        std::cout << "Error7: Variable (" << id->name << ") undeclared on line " << t->line << ":" << t->pos << '\n';
        errors = true;
    } else {
        // Set the isInit to true
        n->st[id->name]->isInit = true;
        // Since it is in current or parent scope, check the expression
        checkExpr(val);
    }
}

void checkWhile(Tree::Node *node) {
    checkBoolExpr(node->children.at(0));
}

void checkIf(Tree::Node *node) {
    checkBoolExpr(node->children.at(0));
}

void checkPrint(Tree::Node *node) {
    checkExpr(node->children.at(0));
}

void checkExpr(Tree::Node *node) {
    std::string name = node->name;
    if (name == "Add") {
        checkAdd(node);
    } else if (name == "Is Equal" || name == "Not Equal") {
        checkBoolExpr(node);
    } else if (node->token != nullptr) {
        Token *t = (Token*)node->token;
        if (t->type == Token::Grammar::ID) {
            SymbolTable::Node *n = findSymbol(node);
            if (n == nullptr) {
                std::cout << "Error: Variable (" << node->name << ") undeclared on line " << t->line << '\n';
                errors = true;
                return;
            }
            n->st[node->name]->isUsed = true;
        }
    }
}

/**
 * @brief According to the grammar any addition must be a digit + expr and the tyoe system
 * states that you can only add an int to an int. This means that every leaf node in addition statements
 * must be of type node and that only the last or right most leaf node can be anything other than a digit.
 * So we simply have to check the addition statemtnts to make sure all leaf nodes are of type int.
 * 
 * @param node 
 */
void checkAdd(Tree::Node *node) {
    Token *child1Token = (Token*)node->children.at(0)->token;
    Tree::Node *child2 = node->children.at(1);

    // If token is null that means child2 is not a leaf node so we need to make sure it is an add
    if (child2->token == nullptr) {
        if (child2->name != "Add") {
            std::cout << "Error6: Incompatible types on line " << child1Token->line << ":" << child1Token->pos <<
                " can't add types int and " << getType(child2) << '\n';
            errors = true;
            return;
        }

        // checkAdd if it is an add statement
        checkAdd(child2);
        return;
    }

    // Since there is a token first check if it is a digit, if it is do nothing
    // If it is an id check its type and if it is anything else print an error
    Token *child2Token = (Token*)child2->token;
    if (child2Token->type != Token::Grammar::DIGIT && child2Token->type != Token::Grammar::ID) {
        std::cout << "Error5: Incompatible types on line " << child2Token->line << " can't add types int and " << 
            Token::grammarToString(child2Token->type) << '\n';
        errors = true;
    } else {
        // Now check if it is an id, if it is we need to first check if it was declared and then if it is an int
        if (child2Token->type == Token::Grammar::ID) {
            SymbolTable::Node *n = findSymbol(child2);
            if (n == nullptr) {
                std::cout << "Error:4 Undeclared variable (" << child2->name << ") on line " <<
                    child2Token->line << ":" << child2Token->pos << '\n';
                errors = true;
                return;
            } else if (getType(child2) != "int") {
                std::cout << "Error3: Incompatible types on line " << child2Token->line << " can't add types int and " << 
                    Token::grammarToString(child2Token->type) << '\n';
                errors = true;
                return;
            }
            n->st[child2->name]->isUsed = true;
        }
    }
}

void checkBoolExpr(Tree::Node *node) {
    if (node->children.size() > 1) {
        std::string child1Type = getType(node->children.at(0));
        std::string child2Type = getType(node->children.at(1));
        if (child1Type != child2Type) {
            Token *t = (Token*)node->children.at(0)->token;
            std::cout << "Error2: Type mismatch on line " << t->line << " can't compare values of type " <<
                child1Type << " and " << child2Type << '\n';
            errors = true;
        }
    } else if (node->children.size() == 1) {
        std::string type = getType(node->children.at(0));
        if (type != "boolean") {
            Token *t = (Token*)node->children.at(0)->token;
            std::cout << "Error1: Invalid type on line " << t->line << " literals in a boolean expression " <<
                "must be of type boolean not " << type << '\n';
            errors = true;
        }
    }
}

/**
 * @brief This function checks if a symbol is in the current or parent scopes of the symbol table. If it finds
 * the symbol, it returns a pointer to that node. If it doesn't exist, it returns a nullptr
 * 
 * @param id Node of the symbol to be found
 * @return SymbolTable::Node* pointer to the node of symbol table or nullptr
 */
SymbolTable::Node* findSymbol(Tree::Node *id) {
    std::string name = id->name;
    // Pointer to current node of symbol table to reset after upwards traversal (if needed)
    SymbolTable::Node *cur = symbol.current;
    // Initilize return value to nullptr for if the symbol can't be found
    SymbolTable::Node *n = nullptr;

    while (symbol.current->parent != nullptr) {
        if (symbol.current->st.find(name) != symbol.current->st.end()) {
            n = symbol.current;
            break;
        } else
            symbol.current = symbol.current->parent;
    }

    symbol.current = cur;
    return n;
}


/**
 * @brief This function gets the type of a node. For add statements it returns int because you can only
 * add ints together and bool exprs return boolean.
 * 
 * @param node 
 * @return std::string representation of the type (int, boolean, or string)
 */
std::string getType(Tree::Node* node) {
    if (node->token != nullptr) {
        Token *t = (Token*)node->token;
        if (t->type == Token::Grammar::ID) {
            SymbolTable::Node *n = findSymbol(node);
            n->st[node->name]->isUsed = true;
            return n->st[node->name]->type;
        } else if (t->type == Token::Grammar::DIGIT) {
            return "int";
        } else if(t->type == Token::Grammar::STRING) {
            return "string";
        } else if (t->type == Token::Grammar::BOOL_VAL) {
            return "boolean";
        }
    } else if (node->name == "Add")
        return "int";
    else if (node->name == "Is Equal" || node->name == "Not Equal")
        return "boolean";
    
    // This error should never return but is here for error checking and so the compiler doesnt
    // scream about all paths not having a return
    return "error";
}


/**
 * @brief This function prints the symbol table
 * 
 * @param node node of symbol table to print
 */
std::vector<std::string> printSymbolTable(SymbolTable::Node *node) {
    // Pointer to string vector to hold the warnings
    std::vector<std::string> warnings;
    if (!node->st.empty()) {
        std::unordered_map<std::string, SymbolTable::SymbolNode*>::iterator itr;
        for (itr = node->st.begin(); itr != node->st.end(); itr++) {
            Token *t = (Token*)itr->second->token;
            bool init = itr->second->isInit;
            bool used = itr->second->isUsed;
            // Add warnings to temp vector for later output
            if (!init) {
                warnings.push_back("Warning: Variable (" + itr->first + ") is declared on line " + 
                    std::to_string(t->line) + " but uninitialized");
            } else if (!used && init) {
                warnings.push_back("Warning: Variable (" + itr->first + ") is initialized but unused");
            } else if (!init && used) {
                warnings.push_back("Warning: Variable (" + itr->first + ") is used but uninitialized");
            }

            // Finally actually print the value of the sybol table
            std::cout << itr->first << "    " << itr->second->type << "      " << itr->second->scope <<
                "       " << t->line << '\n';
        }
    }
    // Recursivley call function for all the children of the node
    for (auto child: node->children) {
        std::vector<std::string> s = printSymbolTable(child);
        warnings.insert(warnings.end(), s.begin(), s.end());
    }

    return warnings;
}