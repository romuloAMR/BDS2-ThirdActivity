#ifndef BTREE_HPP
#define BTREE_HPP

#include <vector>
#include <algorithm>
#include <iostream>

template <typename T, int ORDER>
class BTreeNode {
public:
    std::vector<T> keys;
    std::vector<BTreeNode*> children;
    bool leaf;
    int size;

    BTreeNode(bool isLeaf) : leaf(isLeaf), size(0) {
        keys.resize(2 * ORDER);
        children.resize(2 * ORDER + 1, nullptr);
    }

    ~BTreeNode() {
        for (auto child : children) {
            if (child) delete child;
        }
    }

    void print() {
        for (int i = 0; i < size; ++i) {
            std::cout << keys[i] << " ";
        }
        std::cout << std::endl;
        if (!leaf) {
            for (int i = 0; i <= size; ++i) {
                if (children[i]) children[i]->print();
            }
        }
    }
};

template <typename T, int ORDER>
class BTree {
private:
    BTreeNode<T, ORDER>* root;

    void split(BTreeNode<T, ORDER>* parent, BTreeNode<T, ORDER>* node, int indexFullChild) {
        BTreeNode<T, ORDER>* newNode = new BTreeNode<T, ORDER>(node->leaf);
        int mid = ORDER;

        newNode->size = ORDER;
        for (int i = 0; i < ORDER; ++i) {
            newNode->keys[i] = node->keys[mid + 1 + i];
        }
        if (!node->leaf) {
            for (int i = 0; i <= ORDER; ++i) {
                newNode->children[i] = node->children[mid + 1 + i];
            }
        }

        node->size = ORDER;

        for (int i = parent->size; i > indexFullChild; --i) {
            parent->children[i + 1] = parent->children[i];
        }
        parent->children[indexFullChild + 1] = newNode;

        for (int i = parent->size - 1; i >= indexFullChild; --i) {
            parent->keys[i + 1] = parent->keys[i];
        }

        parent->keys[indexFullChild] = node->keys[mid];
        parent->size++;
    }

    void insertAux(BTreeNode<T, ORDER>* parent, BTreeNode<T, ORDER>* node, int indexFullChild, T key) {
        int i = node->size - 1;
        if (node->leaf) {
            std::vector<T> backup = node->keys;
            while (i >= 0 && key <= node->keys[i]) {
                if (key == node->keys[i]) {
                    node->keys = backup;
                    return;
                }
                node->keys[i + 1] = node->keys[i];
                i--;
            }
            i++;
            node->keys[i] = key;
            node->size++;
        } else {
            while (i >= 0 && key <= node->keys[i]) {
                if (key == node->keys[i]) {
                    return;
                }
                i--;
            }
            i++;
            insertAux(node, node->children[i], i, key);
        }
        if (node->size == 2 * ORDER + 1) {
            if (parent == nullptr) {
                BTreeNode<T, ORDER>* newRoot = new BTreeNode<T, ORDER>(false);
                newRoot->children[0] = node;
                split(newRoot, node, 0);
                root = newRoot;
            } else {
                split(parent, node, indexFullChild);
            }
        }
    }

    void removeAux(BTreeNode<T, ORDER>* parent, BTreeNode<T, ORDER>* node, int key) {
        if (node == nullptr) return;

        int i = 0;
        while (i < node->size && key > node->keys[i]) {
            i++;
        }

        if (i < node->size && key == node->keys[i]) {
            if (node->leaf) {
                // Caso 1: Remoção de uma folha com mais de d+1 elementos
                if (node->size > ORDER) {
                    node->keys.erase(node->keys.begin() + i);
                    node->size--;
                } else {
                    // Caso 3 e 4: Remoção de uma folha com d elementos
                    auto it = std::find(parent->children.begin(), parent->children.end(), node);
                    size_t index = std::distance(parent->children.begin(), it);

                    BTreeNode<T, ORDER>* rightBrother = (index + 1 < parent->children.size()) ? parent->children[index + 1] : nullptr;
                    BTreeNode<T, ORDER>* leftBrother = (index > 0) ? parent->children[index - 1] : nullptr;

                    if (rightBrother && (rightBrother->size + node->size) < 2 * ORDER) {
                        // Caso 3: Concatenação com o irmão da direita
                        node->keys.push_back(parent->keys[index]);
                        node->keys.insert(node->keys.end(), rightBrother->keys.begin(), rightBrother->keys.end());
                        node->size += rightBrother->size + 1;

                        parent->keys.erase(parent->keys.begin() + index);
                        parent->children.erase(parent->children.begin() + index + 1);
                        parent->size--;

                        delete rightBrother;
                    } else if (leftBrother && (leftBrother->size + node->size) < 2 * ORDER) {
                        // Caso 3: Concatenação com o irmão da esquerda
                        leftBrother->keys.push_back(parent->keys[index - 1]);
                        leftBrother->keys.insert(leftBrother->keys.end(), node->keys.begin(), node->keys.end());
                        leftBrother->size += node->size + 1;

                        parent->keys.erase(parent->keys.begin() + index - 1);
                        parent->children.erase(parent->children.begin() + index);
                        parent->size--;

                        delete node;
                    } else if (rightBrother && (rightBrother->size + node->size) >= 2 * ORDER) {
                        // Caso 4: Redistribuição com o irmão da direita
                        node->keys.push_back(parent->keys[index]);
                        parent->keys[index] = rightBrother->keys[0];
                        rightBrother->keys.erase(rightBrother->keys.begin());
                        rightBrother->size--;
                    } else if (leftBrother && (leftBrother->size + node->size) >= 2 * ORDER) {
                        // Caso 4: Redistribuição com o irmão da esquerda
                        node->keys.insert(node->keys.begin(), parent->keys[index - 1]);
                        parent->keys[index - 1] = leftBrother->keys.back();
                        leftBrother->keys.pop_back();
                        leftBrother->size--;
                    }
                }
            } else {
                // Caso 2: Remoção de uma chave de um nó não folha
                BTreeNode<T, ORDER>* leftChild = node->children[i];
                BTreeNode<T, ORDER>* rightChild = node->children[i + 1];

                if (leftChild->size > ORDER) {
                    // Substituir pelo antecessor
                    T predecessor = getPredecessor(leftChild);
                    node->keys[i] = predecessor;
                    removeAux(node, leftChild, predecessor);
                } else if (rightChild->size > ORDER) {
                    // Substituir pelo sucessor
                    T successor = getSuccessor(rightChild);
                    node->keys[i] = successor;
                    removeAux(node, rightChild, successor);
                } else {
                    // Concatenação dos filhos
                    leftChild->keys.push_back(node->keys[i]);
                    leftChild->keys.insert(leftChild->keys.end(), rightChild->keys.begin(), rightChild->keys.end());
                    leftChild->size += rightChild->size + 1;

                    node->keys.erase(node->keys.begin() + i);
                    node->children.erase(node->children.begin() + i + 1);
                    node->size--;

                    delete rightChild;
                    removeAux(parent, node, key);
                }
            }
        } else {
            if (node->leaf) return;
            removeAux(node, node->children[i], key);
        }
    }

    T getPredecessor(BTreeNode<T, ORDER>* node) {
        while (!node->leaf) {
            node = node->children[node->size];
        }
        return node->keys[node->size - 1];
    }

    T getSuccessor(BTreeNode<T, ORDER>* node) {
        while (!node->leaf) {
            node = node->children[0];
        }
        return node->keys[0];
    }

public:
    BTree() {
        root = new BTreeNode<T, ORDER>(true);
    }

    ~BTree() {
        if (root) delete root;
    }

    void insert(T key) {
        insertAux(nullptr, root, 0, key);
    }

    void remove(T key) {
        removeAux(nullptr, root, key);
    }

    void printTree() {
        if (root->size == 0) {
            std::cout << "Nothing" << std::endl;
        } else {
            root->print();
        }
    }
};

#endif