#ifndef BTREENODE_HPP
#define BTREENODE_HPP

#include <vector>
#include <iostream>

/**
 * @brief Representa um nó de uma Árvore B.
 * 
 * @tparam T Tipo dos elementos armazenados no nó.
 * @tparam ORDER Ordem da árvore B.
 */
template <typename T, int ORDER>
class BTreeNode {
public:

    // Atributos da árvore B
    std::vector<T> keys; //<< Vetor de chaves.
    std::vector<BTreeNode<T, ORDER>*> children; //<< Vetor de ponteiros para os filhos.
    bool leaf; //<< Indica se o nó é uma folha.
    int size; //<< Número atual de chaves armazenadas no nó.

    /**
     * @brief Construtor da classe BTreeNode.
     * @param isLeaf Define se o nó será inicializado como uma folha (true por padrão).
     */
    BTreeNode(bool isLeaf = true)
        : leaf(isLeaf), size(0)
    {
        this->keys.resize(2 * ORDER + 1);        // Aloca espaço para as chaves.
        this->children.resize(2 * ORDER + 2);   // Aloca espaço para os filhos.
    }

    /**
     * @brief Destrutor da classe BTreeNode.
     * Libera a memória alocada para os filhos, caso o nó não seja uma folha.
     */
    ~BTreeNode()
    {
        if (!leaf) {
            for (int i = 0; i <= size; i++) {
                delete children[i];
            }
        }
    }

    /**
     * @brief Imprime o conteúdo do nó no console, incluindo suas chaves e filhos.
     * @param level Nível atual na árvore (usado para identação na impressão).
     */
    void print(int level = 0)
    {
        // Imprime as chaves do nó com identação de acordo com o nível.
        std::cout << std::string(level * 4, ' ');
        std::cout << "Lv " << level << " |-";
        for (int i = 0; i < this->size; i++) {
            std::cout << this->keys[i] << "-";
        }
        std::cout << "|" << std::endl;

        // Imprime recursivamente os filhos, se existirem.
        if (!leaf) {
            for (int i = 0; i <= size; i++) {
                children[i]->print(level + 1);
            }
        }
    }

    /**
     * @brief Retorna a capacidade do nó.
     */

    /**
     * @brief Retorna a quantidade de chaves no nó.
     */
};

#endif
