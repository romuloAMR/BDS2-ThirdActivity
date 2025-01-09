#ifndef BTREENODE_HPP
#define BTREENODE_HPP

#include <vector>
#include <iostream>

template <typename T, int ORDER>
class BTreeNode {
    public:
        // Attributes
        std::vector<T> keys;
        std::vector<BTreeNode<T, ORDER>*> children;
        bool leaf;
        int size;

        // Constructors and Destructors
        BTreeNode(bool isLeaf = true)
            : leaf(isLeaf), size(0)
        {
            this->keys.resize(ORDER);
            this->children.resize(ORDER + 1);
        }
        ~BTreeNode()
        {
            if (!leaf) {
                for (int i = 0; i <= size; i++) {
                    delete children[i];
                }
            }
        }

        // Print
        void print(int level = 0)
        {
            std::cout << std::string(level * 4, ' ');
            std::cout << "Lv "<< level << " |-";
            for (int i = 0; i < this->size; i++) {
                std::cout << this->keys[i] << "-";
            }
            std::cout << "|" << std::endl;

            if (!leaf) {
                for (int i = 0; i <= size; i++) {
                    children[i]->print(level + 1);
                }
            }
        }
        
};

#endif
