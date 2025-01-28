#ifndef BTREE_HPP
#define BTREE_HPP

#include <vector>
#include <algorithm>
#include <fstream>
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

void deleteAux(BTreeNode<T, ORDER>* node, int key, int auxIndex = 0){
    auto it = std::find(node->keys.begin(), node->keys.end(), key);
    size_t index = std::distance(node->keys.begin(), it);
    
    node->keys.erase(node->keys.begin() + index + auxIndex); //< Removemos a chave.
    node->children.erase(node->children.begin() + index + (1 - auxIndex)); //< Removemos o ponteiro para o filho do removido.
    node->size--; //< Diminuímos o tamanho.

    std::sort(node->keys.begin(), node->keys.end());
}

void removeAux(BTreeNode<T, ORDER>* parent, BTreeNode<T, ORDER>* node, int key){
        /**
         * Em todos os casos, iremos ter primeramente o nó.
         */
        parent = nullptr;
        BTreeNode<T, ORDER>* currentNode = search(key, parent);
        if(currentNode == nullptr){
            std::cout << "No key was found.\n";
            return;
        }
        if(currentNode->leaf){
            if(currentNode->size > ORDER){
                deleteAux(currentNode, key);
            } else {
                // buscando irmãos adjacentes.
                auto it = std::find(parent->children.begin(), parent->children.end(), currentNode);
                size_t index = std::distance(parent->children.begin(), it);

                std::cout << "index:" << index; // onde esta o ponteiro de vetores do no filho 
                
                BTreeNode<T, ORDER>* rightBrother = nullptr;
                BTreeNode<T, ORDER>* leftBrother = nullptr;

                if (index > 0 && index < (ORDER * 2) + 1){
                    rightBrother = parent->children[index+1];
                    leftBrother = parent->children[index-1];
                } else if(index == 0){
                    rightBrother = parent->children[index+1];
                }  else if(index == (2 * ORDER) + 1) {
                    leftBrother = parent->children[index-1];
                }

                std::cout << "\n\nCUrP: ";
                    currentNode->print();

                std::cout << "R-Brother" << std::endl;
                rightBrother->print();
                std::cout << std:: endl << "L-Brother" << std::endl;
                leftBrother->print();

                deleteAux(currentNode, key);
                std::cout << "\n\nCUrP: ";
                    currentNode->print();
                std::cout << "size: " << rightBrother->size << std::endl;
                std::cout << "size CUrr: " << currentNode->size << std::endl;
                if((rightBrother->size + currentNode->size) < 2 * ORDER){
                    std::cout << "in right" << std::endl;
                    currentNode->size += rightBrother->size + 1;
                    std::cout << "size CUrr: " << currentNode->size << std::endl;
                    std::cout << "size: " << rightBrother->size << std::endl;
                    currentNode->keys.push_back(parent->keys[index]);
                    currentNode->keys.insert(currentNode->keys.end(), rightBrother->keys.begin(), rightBrother->keys.end());
                    std::sort(parent->keys.begin(), parent->keys.end());
                    std::cout << "CUrP:";
                    currentNode->print();
                    parent->keys.erase(parent->keys.begin() + index);
                    parent->children.erase(parent->children.begin() + index + 1);
                    parent->size--;
                    delete rightBrother;
                    //deleteAux(parent, currentNode->keys[ORDER - 1]);
                } else if((leftBrother->size + currentNode->size) < 2 * ORDER){
                    std::cout << "in left" << std::endl;
                    leftBrother->size += currentNode->size + 1;
                    std::cout << "size CUrr: " << currentNode->size << std::endl;
                    std::cout << "size: " << leftBrother->size << std::endl;
                    
                    leftBrother->keys.push_back(parent->keys[index - 1]);
                    leftBrother->keys.insert(leftBrother->keys.end(), currentNode->keys.begin(), currentNode->keys.end());
                    std::sort(parent->keys.begin(), parent->keys.end());
                    std::cout << "CUrP:";
                    leftBrother->print();
                    std::cout << "Lorder" << leftBrother->keys[ORDER];
                    parent->children[index  + 1] = parent->children[index];
                    parent->keys.erase(parent->keys.begin() + index - 1);
                    parent->children.erase(parent->children.begin() + index);
                    parent->size--;
                }
                
           
            }
            

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