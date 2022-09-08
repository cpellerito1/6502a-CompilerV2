// #include "Compiler.h"
 
// Tree ast;

// // Blueprints
// void cstToAst(Tree::Node*);
// void traverse(Tree::Node*);
// void parseExpr(Tree::Node*);
// void parseIntExpr(Tree::Node*);
// void parseStringExpr(Tree::Node*);
// void parseBoolExpr(Tree::Node*);


// void Compiler::semanticAnalysis(Tree &cst, int programCounter) {
//     // First convert the cst to ast
//     cstToAst(cst.root);
// }

// // Parse the cst to make the ast
// void cstToAst(Tree::Node *root) {
//     // Since the only child of the root is going to be block we can skip straight to the children of that node
//     // First we need to create the root node for the ast, then we can begin the traversal from the block
//     ast.addNode("Block", Tree::Kind::ROOT);
//     traverse(root->children.at(0));
    
// }

// void traverse(Tree::Node *node) {
//     for (auto n: node->children) {
//         if (n->name == "Block") {
//             ast.addNode("Block", Tree::Kind::BRANCH);
//             traverse(n);
//         } else if (n->name == "Variable Declaration") {
//             ast.addNode("Variable Declaration", Tree::Kind::BRANCH);
//             ast.addNode(n->children.at(0)->name, Tree::Kind::LEAF);
//             ast.addNode(n->children.at(1)->name, Tree::Kind::LEAF);

//         } else if (n->name == "Assignment Statement") {
//             ast.addNode("Assignment Statement", Tree::Kind::BRANCH);
//             ast.addNode(n->children.at(0)->name, Tree::Kind::LEAF);
//             parseExpr(n->children.at(1));
//         } else if (n->name == "Print Statement") {
//             ast.addNode("Print Statement", Tree::Kind::BRANCH);
//             traverse(n);
//         } else if (n->name == "While Statement") {
//             ast.addNode("While Statement", Tree::Kind::BRANCH);
//             traverse(n);
//         } else if (n->name == "If Statement") {
//             ast.addNode("If Statement", Tree::Kind::BRANCH);
//             traverse(n);
//         } else if (n->name == "Integer Expression") {

//         } else {

//             if (!n->children.empty())
//                 traverse(n);
//         }
//     }

// }


// void parseExpr(Tree::Node *current) {
//     Tree::Node *child = current->children.at(0);
//     if (child->name == "Integer Expression")
//         parseIntExpr(child);
//     else if (child->name == "String Expression")
//         parseStringExpr(child);
//     else if (child->name == "Boolean Expression")
//         parseBoolExpr(child);
    
// }

// std::string parseIntExpr(Tree::Node *current, std::string equation = "") {
//     for (auto n: current->children) {
//         if ()
//     }


//     if (current->children.size() == 1) {
//         // If the int expr only has 1 child it must be a digit, so go to the child of the child of the int expr
//         // to find the leaf node representing the digit
//         Tree::Node *digit = current->children.at(0)->children.at(0);
//         ast.addNode(current->children.at(0)->name, Tree::Kind::LEAF);
//         // Then check to see if the equaiton is empty, if it is we can just add the digit node and if it isnt
//         // We have to add it to the equation to be output
//         if (equation != "")
//             equation += current->children.at(0)->name;
//     } else {


//     }

//     return equation;
// }

// void parseStringExpr(Tree::Node *current) {

// }