#include "Compiler.h"

// Load accumulator with constant and from memory
#define LDA_C "A9"
#define LDA_M "AD"
// Store the accumulator in memory
#define STA "8D"
// Add with carry
#define ADC "6D"
// Load x register with a constant from memory
#define LDX_C "A2"
#define LDX_M "AE"
// Load the y register with a constant and from memory
#define LDY_C "A0"
#define LDY_M "AC"
// No operation/end of program (EOP)
#define EOP "EA"
// Break/System call
#define BRK "00"
// Compare a byte in memory to the x register, sets the z flag to 1 if equal
#define CMX "EC"
// Branch N bytes if z flag = 0
#define BNE "D0"
// Increment the value of a byte
#define INC "EE"
// System call
#define SYS "FF"
// Null pointer, same as system call but used differently so two seperate consts helps with clarity
#define NPTR "FF"

// Global variables
Tree ast;
SymbolTable symbol;
bool errors;
int scope;

// 256 byte executable image
std::string exec[256];
// Current pointer for exec
int current;
// Pointer to the location of the heap in exec
int heap;

// Hashmap to keep track of static variable
std::unordered_map<std::string, std::string> tempStatic;
int temp;

// Hashmap to keep track of jumps
std::unordered_map<std::string, int> jump;
int j;

// Hashmap to keep track of strings, this allows the compiler to assign like strings the same pointer to save sapce in the heap
std::unordered_map<std::string, std::string> stringTable;

std::vector<int> bool_location;

// Blueprints
void traverse(Tree::Node*);
void genBlock(Tree::Node*);
void genVarDecl(Tree::Node*);
void genAssign(Tree::Node*);
void genExpr(Tree::Node*);
void genAdd(Tree::Node*);
void genEqual(Tree::Node*);

void incrementCurrent(void);

void Compiler::codeGen(Tree &t, SymbolTable &s, int &programCounter) {
    // Initialize global variables
    ast = t;
    symbol = s;
    current = 0;
    heap = 254; // Last value of exec is always null so heap starts here
    temp = 0;
    j = 0;

    std::cout << "Beginning code gen for program " << programCounter << '\n';
    // Initialize the exec image. 00 for everything except the last byte
    for (int i = 0; i < 256; i++) 
        exec[i] = (i == 255)? NPTR : "00";
    
    traverse(ast.root);


}


void traverse(Tree::Node *node) {
    std::string name = node->name;
    if (name == "Block") {
        genBlock(node);
    } else if (name == "Variable Declaration") {
        genVarDecl(node);
    } else if (name == "Assignment Statement") {
        genAssign(node);
    } else if (name == "While Statement" || name == "If Statement") {
        genExpr(node->children.at(0));
    } else if (name == "Print Statement") {
        genExpr(node->children.at(0));
    }

    if (!node->children.empty()) {
        for (auto c: node->children)
            traverse(c);
    }
}


void genVarDecl(Tree::Node *node) {
    Tree::Node *var = node->children.at(1);
    Tree::Node *type = node->children.at(0);
    // Load the accumulator with constant
    exec[current] = LDA_C;
    incrementCurrent();
    // Figure out what to load the accumulator with. int and bool default to 00 and string to nullptr (FF)
    if (type->name == "int" || type->name == "boolean") {
        exec[current] = "00"; // Technically don't need this because everything should be init to 00 but better to be safe
        incrementCurrent();
    } else {
        // We know this has to be string since it made it through Lex, Parse and SA
        exec[current] = NPTR;
        incrementCurrent();
    }

    // Store the var in a temp location and add that location to the temp hashmap
    exec[current] = STA;
    incrementCurrent();
    exec[current] = "T" + temp;
    incrementCurrent();
    exec[current] = "XX";
    incrementCurrent();
    tempStatic[var->name + ":" + std::to_string(scope)] =  "T" + temp;
    temp++;
}


// Gen Assignment Statement
void genAssign(Tree::Node *node) {
    Tree::Node *var = node->children.at(0);
    Tree::Node *value = node->children.at(1);
    if (value->name == "Add"){
        genAdd(value);
        exec[current] = STA;
        incrementCurrent();
        exec[current] = findTemp(var, scope);
        incrementCurrent();
        exec[current] = "XX";
        incrementCurrent();
    } else if (value->name == "Is Equal" || value->name == "Not Equal"){
        genEqual(value);
    } else if (value->token != nullptr && value->token->type == Token::Grammar::ID){
        exec[current] = LDA_M;
        incrementCurrent();
        exec[current] = findTemp(value, scope);
        incrementCurrent();
        exec[current] = "XX";
        incrementCurrent();
    } else {
        exec[current] = LDA_C;
        incrementCurrent();
        if (getType(var) == "int") {
            exec[current] = "0" + value->name;
            incrementCurrent();
        } else if (getType(var) == "boolean") {
            if (value->name == "true")
                exec[current] = "01";
            else
                exec[current] = "00";
            bool_location.push_back(current);
            incrementCurrent();
        } else {
            exec[current] = setString(value);
            incrementCurrent();
        }
    }
    exec[current] = "8D";
    incrementCurrent();
    exec[current] = findTemp(var, scope);
    incrementCurrent();
    exec[current] = "XX";
    incrementCurrent();
}


void genPrint(Tree::Node *node) {
    if (node->children.at(0)->name == "Add"){
            exec[current] = LDY_M;
            incrementCurrent();
            genAdd(node->children.at(0));
        } else if (node->children.at(0)->name == "Is Equal" || node->children.at(0)->name == "Not Equal"){
            exec[current] = "AC";
            incrementCurrent();
            genEqual(node->children.at(0));
        } else if (node->children.at(0)->token != nullptr) {
            if (node->children.at(0)->token->type == Token::Grammar::ID){
                exec[current] = "AC";
                incrementCurrent();
                exec[current] = findTemp(node->children.at(0), scope);
                incrementCurrent();
                exec[current] = "XX";
                incrementCurrent();
                exec[current] = "A2";
                incrementCurrent();
                if (getType(node->children.at(0)).equals("string"))
                    exec[current] = "02";
                else {
                    exec[current] = "01";
                    if (getType(node->children.at(0)).equals("boolean"))
                        boolPrint.add(current);
                }
                incrementCurrent();
            } else if (node->children.at(0)->token->type == Token::Grammar::DIGIT) {
                exec[current] = "A0";
                incrementCurrent();
                exec[current] = "0" + node->children.at(0)->name;
                incrementCurrent();
                exec[current] = "A2";
                incrementCurrent();
                exec[current] = "01";
                incrementCurrent();
            } else if (node->children.at(0).token.type == Token::Grammar::BOOL_VAL) {
                exec[current] = "A0";
                incrementCurrent();
                if (node->children.at(0)->name == "true")
                    exec[current] = "01";
                else
                    exec[current] = "00";
                boolLiteral.add(current);
                incrementCurrent();
                exec[current] = "A2";
                incrementCurrent();
                exec[current] = "01";
                boolPrint.add(current);
                incrementCurrent();
            } else if (node->children.at(0)->token->type == Token::Grammar::STRING) {
                exec[current] = "A0";
                incrementCurrent();
                exec[current] = setString(node->children.at(0));
                incrementCurrent();
                exec[current] = "A2";
                incrementCurrent();
                exec[current] = "02";
                incrementCurrent();
            }
        }

        exec[current] = SYS;
        //incrementCurrent();
}


void genAdd(Tree::Node *node) {

}


void genEqual(Tree::Node *node) {

}


void incrementCurrent(int value = 1) {
    current += value;

    if (current > 254) {
        std::cout << "Error: Size Limit exceeded\n";
        errors = true;
    }
}