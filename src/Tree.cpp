#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include "Token.h"

class Tree {
    public:
        enum Kind {
            ROOT, BRANCH, LEAF
        };

        struct Node {
            std::string name;
            Node* parent;
            std::vector<Node*> children;
            Token *token;

            Node(std::string name, Token *token = nullptr) {
                this->name = name;
                this->token = token;
            }
        };

        // pointers to root and current nodes
        Node* root;
        Node* current;

        void addNode(std::string name, Kind k, Token *t = nullptr) {
            Node* n = new Node(name, t);
            if (this->root == nullptr) {
                this->root = n;
                this->current = n;
            } else {
                this->current->children.push_back(n);
                n->parent = this->current;
            }

            if (k == Kind::BRANCH)
               this->current = n;
        }

        void moveUp() {
            if (this->current->parent != nullptr)
                this->current = this->current->parent;
        }

        std::string toString() {
            std::string traversalResult = "";
            traversalResult = expand(root, 0, traversalResult);

            return traversalResult;
        }

        std::string expand(Node* node, int depth, std::string traversal) {
            traversal.append(depth, '-');

            if (node->children.empty())
                traversal += "[" + node->name + "]\n";
            else {
                traversal += "<" + node->name + ">\n";

                for (int i = 0; i < node->children.size(); i++)
                    traversal = expand(node->children.at(i), depth + 1, traversal);
            }

            return traversal;
        }

        void restructure() {
            // Create variable for the children of the parent
            std::vector<Node*> &pChildren = this->current->parent->children;
            // Get iterator to the 2nd to last node in pChildren
            std::vector<Node*>::iterator it = std::find(pChildren.begin(), pChildren.end(), this->current) - 1;
            // Then add that element to the current nodes children
            this->current->children.push_back(*it);
            // Finally remove the child you just added to the current nodes children from pChildren
            pChildren.erase(it);
        }

        void clear() {
            this->root = nullptr;
            this->current = nullptr;
        }
};
