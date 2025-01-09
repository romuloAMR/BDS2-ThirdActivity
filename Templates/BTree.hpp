#ifndef BTREE_HPP
#define BTREE_HPP

#include "BTreeNode.hpp"

template <typename T, int ORDER>
class BTree {
private:
    // Attributes
    BTreeNode<T, ORDER>* root;

public:
    // Constructors and Destructors
    BTree();
    ~BTree();

    // Insert
    //void insert(T k);

};

#endif

template <typename T, int ORDER>
BTree<T, ORDER>::BTree() {
    root = nullptr;
}

template <typename T, int ORDER>
BTree<T, ORDER>::~BTree() {
    if (root != nullptr) {
        delete root;
    }
}
