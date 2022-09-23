#include <vector>
#include <unordered_map>
#include <string>
#include "Token.h"


class SymbolTable {
    public:
        struct SymbolNode {
            std::string type;
            int scope;
            Token *token;
            bool isInit = false;
            bool isUsed = false;

            SymbolNode(std::string type, int scope, Token *token) {
                this->type = type;
                this->scope = scope;
                this->token = token;
            }
        };

        struct Node {
            Node* parent = nullptr;
            std::vector<Node*> children;
            std::unordered_map<std::string, SymbolNode*> st;
        };

        Node *root;
        Node *current;

        void addNode() {
            Node *n = new Node;
            if (this->root == nullptr) {
                this->root = n;
                this->current = n;
            } else {
                this->current->children.push_back(n);
                n->parent = this->current;
            }

            this->current = n;
        }

        void moveUp() {
            if (this->current->parent != nullptr)
                this->current = this->current->parent;
        }
};