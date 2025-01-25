#ifndef BTREE_HPP
#define BTREE_HPP

#include "BTreeNode.hpp"

/**
 * @brief Implementação de uma Árvore B genérica.
 * 
 * @tparam T Tipo dos elementos armazenados na árvore.
 * @tparam ORDER Ordem da árvore B.
 */
template <typename T, int ORDER>
class BTree {
private:

    /**
     * @brief Ponteiro para o nó raiz da árvore.
     */
    BTreeNode<T, ORDER>* root;

    /**
     * @brief Realiza a divisão de um nó que está cheio.
     * 
     * @param parent Ponteiro para o nó pai do nó a ser dividido.
     * @param node Ponteiro para o nó a ser dividido.
     * @param indexFullChild Índice do nó filho cheio no nó pai.
     */
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

    /**
     * @brief Auxilia na inserção de uma chave na árvore B.
     * 
     * @param parent Ponteiro para o nó pai do nó atual.
     * @param node Ponteiro para o nó atual.
     * @param indexFullChild Índice do nó filho cheio no nó pai.
     * @param key Chave a ser inserida.
     */
    void insertAux(BTreeNode<T, ORDER>* parent, BTreeNode<T, ORDER>* node, int indexFullChild, T key){
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



public:
    /**
     * @brief Construtor da árvore B.
     */
    BTree() {
        root = new BTreeNode<T, ORDER>(true);
    }

    /**
     * @brief Destrutor da árvore B.
     */
    ~BTree() {
        if (root != nullptr) {
            delete root;
        }
    }

    /**
     * @brief Insere uma chave na árvore B.
     * 
     * @param key Chave a ser inserida.
     */
    void insert(T key) {
        insertAux(nullptr, root, 0, key);
    }

    /**
     * @brief Imprime a árvore B.
     */
    void printTree() {
        if (root->size == 0) {
            std::cout << "Nothing" << std::endl;
        } else {
            root->print();
        }
    }
};

#endif
