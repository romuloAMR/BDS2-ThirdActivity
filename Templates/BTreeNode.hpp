#ifndef BTREENODE_HPP
#define BTREENODE_HPP

#include <vector>

template <typename T, int ORDER>
class BTreeNode {
    public:
        // Attributes
        std::vector<T> keys;
        std::vector<BTreeNode<T, ORDER>*> children;
        bool leaf;
        int size;

        // Constructors and Destructors
        BTreeNode(bool isLeaf = true);
        ~BTreeNode();
};

#endif

// Constructors and Destructors
template <typename T, int ORDER>
inline BTreeNode<T, ORDER>::BTreeNode(bool isLeaf)
{
    this->leaf = isLeaf;
    this->size = 0;
    this->keys.resize(ORDER);
    this->children.resize(ORDER + 1, nullptr);
}
template <typename T, int ORDER>
BTreeNode<T, ORDER>::~BTreeNode()
{
    // Nothing
}

