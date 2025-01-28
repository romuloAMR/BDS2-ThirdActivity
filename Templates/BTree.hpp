#ifndef BTREE_HPP
#define BTREE_HPP

#include <vector>
#include <algorithm>
#include <fstream>
#include <string.h>
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

    // Função para buscar uma chave na árvore
    BTreeNode<T, ORDER>* search(int key, BTreeNode<T, ORDER>*& parent) {
    BTreeNode<T, ORDER>* currentNode = root;
    parent = nullptr;

    while (currentNode != nullptr) {
        // Procura a chave no nó atual
        auto it = std::lower_bound(currentNode->keys.begin(), currentNode->keys.end(), key);
        unsigned int index = std::distance(currentNode->keys.begin(), it);

        // Verifica se encontramos a chave
        if (it != currentNode->keys.end() && *it == key) {
            return currentNode;
        }

        // Atualiza o pai e segue para o próximo nó
        parent = currentNode;
        currentNode = (index < currentNode->children.size()) ? currentNode->children[index] : nullptr;
    }

    return nullptr; // Chave não encontrada
}

void removeAux(BTreeNode<T, ORDER>* parent, BTreeNode<T, ORDER>* node, int key){
    /**
     * Em todos os casos, iremos ter primeramente o nó.
     */
    BTreeNode<T, ORDER>* currentParent = nullptr;
    BTreeNode<T, ORDER>* currentNode = search(key, currentParent);
    if(currentNode == nullptr){
        std::cout << "No key was found.\n";
        return;
    }

    //
    // Primeiro caso:
    // Estamos removendo de um folha.
    //

    if(currentNode->leaf){
        if(currentNode->size > ORDER + 1){
        // CASO 1:
        // Removemos de uma folha com d + 1 elementos.
            
           //< Vamos calcular a posição de índice da chave para remover diretamente ela.
           auto it = std::find(currentNode->keys.begin(), currentNode->keys.end(), key);
           size_t index = std::distance(currentNode->keys.begin(), it);

           //< Removemos a chave (index) e o ponteiro dela (index + 1). Diminuímos o tamanho.
           currentNode->keys.erase(currentNode->keys.begin() + index); //< Removemos a chave.
           currentNode->children.erase(currentNode->children.begin() + index + 1); //< Removemos o ponteiro para o filho do removido.
           currentNode->size--; //< Diminuímos o tamanho.

           std::sort(currentNode->keys.begin(), currentNode->keys.end()); //< Ordenamos as chaves.

        } else if (currentNode->size == ORDER){ // Tenho que achar um jeito de puxar os irmãos.
            /*
             * Caso 3: a folha tem d elementos mas possui uma irmã adjacente
             * e as duas juntas possuem menos de 2d elementos
             */

            // Cálculo dos nós irmãos adjacentes.
            auto it = std::find(parent->children.begin(), parent->children.end(), currentNode);
            size_t index = std::distance(parent->children.begin(), it);
            BTreeNode<T, ORDER>* rightBrother = (index + 1 < parent->children.size()) ? parent->children[index + 1] : nullptr;
            BTreeNode<T, ORDER>* leftBrother = (index > 0) ? parent->children[index - 1] : nullptr;

            if((leftBrother && leftBrother->size + currentNode->size <= 2 * ORDER) || 
               (rightBrother && rightBrother->size + currentNode->size <= 2 * ORDER)) {
                if(leftBrother && leftBrother->size + currentNode->size <= 2 * ORDER) {
                    // Fundir com o irmão da esquerda
                    leftBrother->keys.insert(leftBrother->keys.end(), currentNode->keys.begin(), currentNode->keys.end());
                    leftBrother->children.insert(leftBrother->children.end(), currentNode->children.begin(), currentNode->children.end());
                    leftBrother->size += currentNode->size;

                    parent->keys.erase(parent->keys.begin() + index - 1);
                    parent->children.erase(parent->children.begin() + index);
                    parent->size--;
                } else if(rightBrother) {
                    // Fundir com o irmão da direita
                    currentNode->keys.insert(currentNode->keys.end(), rightBrother->keys.begin(), rightBrother->keys.end());
                    currentNode->children.insert(currentNode->children.end(), rightBrother->children.begin(), rightBrother->children.end());
                    currentNode->size += rightBrother->size;

                    parent->keys.erase(parent->keys.begin() + index);
                    parent->children.erase(parent->children.begin() + index + 1);
                    parent->size--;
                }
            } else {
                /*
                 * Caso 4: A folha tem d elementos e possui um irmão adjacente
                 * com pelo menos 2d elementos. Realizamos uma rotação.
                 */

                if(leftBrother && leftBrother->size > ORDER) {
                    // Rotação para a direita
                    currentNode->keys.insert(currentNode->keys.begin(), parent->keys[index - 1]);
                    parent->keys[index - 1] = leftBrother->keys.back();
                    leftBrother->keys.pop_back();

                    if(!leftBrother->leaf) {
                        currentNode->children.insert(currentNode->children.begin(), leftBrother->children.back());
                        leftBrother->children.pop_back();
                    }

                    currentNode->size++;
                    leftBrother->size--;
                } else if(rightBrother && rightBrother->size > ORDER) {
                    // Rotação para a esquerda
                    currentNode->keys.push_back(parent->keys[index]);
                    parent->keys[index] = rightBrother->keys.front();
                    rightBrother->keys.erase(rightBrother->keys.begin());

                    if(!rightBrother->leaf) {
                        currentNode->children.push_back(rightBrother->children.front());
                        rightBrother->children.erase(rightBrother->children.begin());
                    }

                    currentNode->size++;
                    rightBrother->size--;
                }
            }
        }
    } else { // Aqui quer dizer que o nó não é uma folha
        /**
         * Se o nó não for uma folha, isso quer dizer que ele tem um 
         * ou mais filhos. Logo, podemos fazer algumas brincadeiras.
         * 
         * A primeira coisa que quero fazer é avaliar qual dois dois filhos da 
         * chave que será removida é ver qual dos dois tem mais elementos.
         * O que tiver mais elementos, nós tiramos, colocamos no local do pai
         * e depois removemos o vetor novamente. Perceba que disso estaremos perdendo
         * um dos ponteiros de filhos do lado que iremos remover.
         */
        // Cálculo dos nós irmãos adjacentes.
        auto it = std::find(parent->children.begin(), parent->children.end(), currentNode);
        size_t index = std::distance(parent->children.begin(), it);

        // Verificar os limites para evitar acesso inválido
        BTreeNode<T, ORDER>* leftBrother = (index > 0) ? parent->children[index - 1] : nullptr;
        BTreeNode<T, ORDER>* rightBrother = (index + 1 < parent->children.size()) ? parent->children[index + 1] : nullptr;

        // Escolher o irmão preferido com base no tamanho (garantindo que eles existam)
        int chosenBrotherIndex = -1;
        BTreeNode<T, ORDER>* chosenBrother = nullptr;
        if (leftBrother && rightBrother){
            chosenBrother = (rightBrother->size > leftBrother->size) ? rightBrother : leftBrother;
        } else if (leftBrother){
            chosenBrother = leftBrother; // Só tem o irmão da esquerda
            chosenBrotherIndex = leftBrother->keys.size() - 1; // Última posição
        } else if (rightBrother){
            chosenBrother = rightBrother; // Só tem o irmão da direita
            chosenBrotherIndex = 0; // Primeira posição
        }

        // Trocamos a chave com o irmão escolhido
        std::swap(currentNode->keys[index], chosenBrother->keys[chosenBrotherIndex]);
        chosenBrother->keys.erase(chosenBrother->keys.begin() + chosenBrotherIndex);
        chosenBrother->size--;

        // Ordenar
        std::sort(chosenBrother->keys.begin(), chosenBrother->keys.end());
    }
}



    void generateDotAux(BTreeNode<T, ORDER>* node, std::ofstream& dotFile) {
        if (!node) return;

        // Cria um nó no arquivo .dot
        dotFile << "node" << node << " [label=\"";
        for (int i = 0; i < node->size; ++i) {
            dotFile << node->keys[i];
            if (i < node->size - 1) dotFile << " | ";
        }
        dotFile << "\"];\n";

        // Conecta o nó aos seus filhos
        if (!node->leaf) {
            for (int i = 0; i <= node->size; ++i) {
                if (node->children[i]) {
                    dotFile << "node" << node << " -> node" << node->children[i] << ";\n";
                    generateDotAux(node->children[i], dotFile);
                }
            }
        }
    }

public:
    BTree() {
        root = new BTreeNode<T, ORDER>(true);
    }

    ~BTree() {
        if (root) delete root;
    }

    void generateDotFile(const std::string& filename) {
        std::ofstream dotFile(filename);
        if (!dotFile.is_open()) {
            std::cerr << "Erro ao abrir o arquivo .dot\n";
            return;
        }

        dotFile << "digraph BTree {\n";
        dotFile << "node [shape=record];\n";
        generateDotAux(root, dotFile);
        dotFile << "}\n";

        dotFile.close();
        std::cout << "Arquivo .dot gerado com sucesso: " << filename << std::endl;
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

    BTreeNode<T, ORDER>* search(int key){
        search(key, this->root);
    }
};

#endif