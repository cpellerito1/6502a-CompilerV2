#include "Compiler.h"
#include <regex>

// Global variables
static Tree ast;
int scope;
bool errors;
//Symbol Table
Tree symbol;

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
void* findSymbol(Tree::Node*);
bool checkType(Tree::Node*, std::string);
std::string getType(Tree::Node*);


void Compiler::semanticAnalysis(Tree &t, int programCounter) {
    ast = t;
    scope = 0;
    errors = false;
    // Traverse the AST starting from root
    traverse(ast.root);
}


void traverse(Tree::Node *node) {
    std::string name = node->name;
    if (name == "Block") {
        checkBlock(node);
    } else if (name == "Variable Declaration") {
        checkVarDecl(node);
    } else if (name == "Assignent Statement") {
        checkAssign(node);
    } else if (name == "While Statement") {
        checkWhile(node);
    } else if (name == "If Statement") {
        checkIf(node);
    } else if (name == "Print Statement") {
        checkPrint(node);
    }

    if (node->children.empty()) {
        for (auto c: node->children)
            traverse(c);
    }

    // Decrement scope after traversing the children of the block
    if (name == "Block")
        scope--;
}

void checkBlock(Tree::Node *node) {
    scope++;
    std::string s(scope, 1);
    if (symbol.root == nullptr) {
        symbol.addNode(s, Tree::Kind::ROOT);
    } else {
        symbol.addNode(s, Tree::Kind::BRANCH);
    }
}

void checkVarDecl(Tree::Node *node) {
    Tree::Node *type = node->children.at(0);
    Tree::Node *id = node->children.at(1);

    // Check if variable is in symbol table in the current scope
    if (symbol.current->st.find(id->name) == symbol.current->st.end()) {
        Token *t = (Token*)id->token;
        std::cout << "Error: Variable (" << t->value << ") already decalred in this scope on line " << 
            t->line << ":" << t->pos << '\n';
        errors = true;
    } else {
        symbol.current->st[id->name] = new Tree::Node(type->name, id->token);
    }
}

void checkAssign(Tree::Node *node) {
    Tree::Node *id = node->children.at(0);
    Tree::Node *val = node->children.at(1);

    // Check if id is in current or parent scope
    void *s = findSymbol(id);
    if (s == nullptr) {
        Token *t = (Token*)id->token;
        std::cout << "Error: Variable (" << id->name << ") undeclared on line " << t->line << ":" << t->pos << '\n';
    } else {
        // Since it is in current or parent scope, check the expression
        checkExpr(val);
        // std::string type = getType(val);
        // // Convert s to Node*
        // Tree::Node *np = (Tree::Node*)s;
        // if (!checkType(id, type)) {
        //     Token *t = (Token*)id->token;
        //     std::cout << "Error: Type mismatch on line " << t->line << ":" << t->pos << " can't assign type (" << 
        //         type << ") to variable of type (" << np->name << ")\n";
        // }
    }
}

void checkWhile(Tree::Node *node) {

}

void checkIf(Tree::Node *node) {

}

void checkPrint(Tree::Node *node) {

}

void checkExpr(Tree::Node *node) {
    std::string name = node->name;
    if (name == "Add") {
        checkAdd(node);
    } else if (name == "Is Equal" || name == "Not Equal") {
        checkBoolExpr(node);
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
            std::cout << "Error: Incompatible types on line " << child1Token->line << ":" << child1Token->pos <<
                " can't add types int and " << getType(child2) << '\n';
            errors = true;
        } else {
            checkAdd(child2);
        }
    } else {
        // Since there is a token first check if it is a digit, if it is do nothing
        // If it is an id check its type and if it is anythin else print an error
        Token *child2Token = (Token*)child2->token;
        if (child2Token->type != Token::Grammar::DIGIT || child2Token->type != Token::Grammar::ID) {
            std::cout << "Error: Incompatible types on line " << child2Token->line << " can't add types int and " << 
                Token::grammarToString(child2Token->type) << '\n';
            errors = true;
        } else {
            // Now check if it is an id, if it is we need to first check if it was declared and then if it is an int
            if (child2Token->type == Token::Grammar::ID) {
                if (findSymbol(child2) == nullptr) {
                    std::cout << "Error: Undeclared variable (" << child2->name << ") on line " <<
                        child2Token->line << ":" << child2Token->pos << '\n';
                    errors = true;
                } else if (getType(child2) != "int") {
                    std::cout << "Error: Incompatible types on line " << child2Token->line << " can't add types int and " << 
                        Token::grammarToString(child2Token->type) << '\n';
                    errors = true;
                }
            }
        }
    }
}

void checkBoolExpr(Tree::Node *node) {

}

/**
 * @brief This function looks at the current scope and then parent scopes in the symbol table to try
 * and find a variable.
 * 
 * @param id Node of the id
 * @return void* pointer to either the node of the symbol table if it exists or nullptr if not 
 */
void* findSymbol(Tree::Node *id) {
    std::string name = id->name;
    Tree::Node *n = symbol.current;

    while (n->parent != nullptr) {
        if (n->st.find(name) != n->st.end())
            return n;
        else
            n = n->parent;
    }

    return nullptr;
}


bool checkType(Tree::Node* id, std::string type) {
    // Get the node of the symbol table where the id is located
    // Since this will only ever be called after the ID was verified the void* can be cast to a Node*
    Tree::Node *s = (Tree::Node*)findSymbol(id);
    return (s->name == type);
}


std::string getType(Tree::Node* node) {
    if (node->token != nullptr) {
        Token *t = (Token*)node->token;
        if (t->type == Token::Grammar::ID) {
            Tree::Node *n = (Tree::Node*)findSymbol(node);
            return n->st[node->name]->name;
        } else {
            return Token::grammarToString(t->type);
        }
    } else if (node->name == "Add")
        return "int";
    else if (node->name == "Is Equal" || node->name == "Not Equal")
        return "boolean";
    else
        return "string";
}