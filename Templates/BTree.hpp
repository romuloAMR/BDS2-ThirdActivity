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

    

    void removeAux(BTreeNode<T, ORDER>* parent, BTreeNode<T, ORDER>* node, int key){
        /**
         * Em todos os casos, iremos ter primeramente o nó.
         */
        BTreeNode* parent = nullptr;
        BTreeNode* currentNode = search(key, parent);
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

                if (currentNode->size == ORDER){ // Tenho que achar um jeito de puxar os irmãos.
                /*
                * Caso 3: a folha tem d elementos mas possui uma irmã adjacente
                * e as duas juntas possuem menos de 2d elementos.
                */

                    // Cálculo dos nós irmãos adjacentes.
                    auto it = std::find(parent->children.begin(), parent->children.end(), currentNode);
                    size_t index = std::distance(parent->children.begin(), it);
                    BTreeNode<T, ORDER>* rightBrother = &parent->children[index + 1]; // Irmão da direita
                    BTreeNode<T, ORDER>* leftBrother = &parent->children[index - 1]; // Irmão da esquerda

                    if ((rightBrother->size + currentNode->size) < 2 * ORDER && (leftBrother->size + currentNode->size) < 2 * ORDER) {
                        if (rightBrother->size > leftBrother->size) {
                            // O maior nó/página irmão é o da direita, o que indica que iremos remover da esquerda.
                            // Pegamos a menor chave do lado da direita, inserimos no vetor do pai,
                            // removemos o pai e o inserimos no vetor do nó a ter a chave removida.
                            // Removemos a chave do nó e ordenamos o vetor novamente.

                            // Primeiro pegamos o primeiro elemento do irmão da direita.
                            int rightBrotherValueToBeSwapped = rightBrother->keys[0];

                            // Pegamos o último elemento do nó atual.
                            int currentNodeLastKey = currentNode->keys[currentNode->size - 1];

                            // Encontramos o índice e o valor a ser removido do nó atual.
                            size_t index_to_remove = std::distance(currentNode->keys.begin(),
                            std::find(currentNode->keys.begin(), currentNode->keys.end(), key));
                            int* currentNodeValueToBeSwapped = &currentNode->keys[index_to_remove];
                            int valueToBeRemoved = *currentNodeValueToBeSwapped;

                            // Procuramos o valor do pai que é maior que o nó atual e menor que o irmão da direita.
                            int* parentValueToBeSwapped = nullptr;
                            for (size_t i = 0; i < parent->keys.size(); i++) {
                            if (parent->keys[i] > currentNodeLastKey && parent->keys[i] < rightBrotherValueToBeSwapped) {
                                parentValueToBeSwapped = &parent->keys[i];
                                break;
                            }
                        }
                    }
                }

                // Ajustando as mudanças.
                rightBrother->keys.erase(rightBrother->keys.begin()); // Remove o primeiro elemento do irmão direito.
                rightBrother->size--; // Ajusta o tamanho.
                rightBrother->children.pop_back(); // Ajusta os ponteiros dos filhos.

                // Adicionando o valor do irmão direito ao pai.
                parent->keys.push_back(rightBrotherValueToBeSwapped);

                // Trocando o pai com o valor a ser removido.
                std::swap(*currentNodeValueToBeSwapped, *parentValueToBeSwapped);

                // Removendo o valor do pai.
                for (size_t i = 0; i < parent->keys.size(); i++) {
                    if (parent->keys[i] == valueToBeRemoved) {
                        parent->keys.erase(parent->keys.begin() + i);
                        break;
                    }
                }

                // Ordenando os vetores para manter a consistência.
                std::sort(currentNode->keys.begin(), currentNode->keys.end());
                std::sort(parent->keys.begin(), parent->keys.end());
                std::sort(rightBrother->keys.begin(), rightBrother->keys.end());
            } else {
                // O menor irmão é o da esquerda, o que indica que iremos remover da direita.
                // Pegamos a maior chave do lado da esquerda, inserimos no vetor do pai,
                // removemos o pai e o inserimos no vetor do nó a ter a chave removida.
                // Removemos a chave do nó e ordenamos o vetor novamente.

                // Primeiro pegamos o último elemento do irmão da esquerda.
                int leftBrotherValueToBeSwapped = leftBrother->keys[leftBrother->size - 1];

                // Pegamos o primeiro elemento do nó atual.
                int currentNodeFirstKey = currentNode->keys[0];

                // Encontramos o índice e o valor a ser removido do nó atual.
                size_t index_to_remove = std::distance(currentNode->keys.begin(),
                std::find(currentNode->keys.begin(), currentNode->keys.end(), key));
                int* currentNodeValueToBeSwapped = &currentNode->keys[index_to_remove];
                int valueToBeRemoved = *currentNodeValueToBeSwapped;

                // Procuramos o valor do pai que é maior que o irmão da esquerda e menor que o nó atual.
                int* parentValueToBeSwapped = nullptr;
                for (size_t i = 0; i < parent->keys.size(); i++) {
                    if (parent->keys[i] > leftBrotherValueToBeSwapped && parent->keys[i] < currentNodeFirstKey) {
                        parentValueToBeSwapped = &parent->keys[i];
                        break;
                    }
                }

                // Ajustando as mudanças.
                leftBrother->keys.pop_back(); // Remove o último elemento do irmão esquerdo.
                leftBrother->size--; // Ajusta o tamanho.
                leftBrother->children.pop_back(); // Ajusta os ponteiros dos filhos.

                // Adicionando o valor do irmão esquerdo ao pai.
                parent->keys.push_back(leftBrotherValueToBeSwapped);

                // Trocando o pai com o valor a ser removido.
                std::swap(*currentNodeValueToBeSwapped, *parentValueToBeSwapped);

                // Removendo o valor do pai.
                for (size_t i = 0; i < parent->keys.size(); i++) {
                    if (parent->keys[i] == valueToBeRemoved) {
                        parent->keys.erase(parent->keys.begin() + i);
                        break;
                    }
                }

                // Ordenando os vetores para manter a consistência.
                std::sort(currentNode->keys.begin(), currentNode->keys.end());
                std::sort(parent->keys.begin(), parent->keys.end());
                std::sort(leftBrother->keys.begin(), leftBrother->keys.end());
            } 
            else { // Aqui quer dizer que o nó não é uma folha
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
                    chosenBrotherIndex = chosenBrother->size-1;
                } else if (rightBrother){
                    chosenBrother = rightBrother; // Só tem o irmão da direita
                    chosenBrotherIndex = 0;
                }

            // Trocamos a chave com o irmão escolhido
                std::swap(currentNode->keys[index], chosenBrother[chosenBrotherIndex]);
                chosenBrother->keys.erase(chosenBrother->keys.begin() + chosenBrotherIndex);
                chosenBrother->size--;

                // Ordenar
                std::sort(chosenBrother->keys.begin(), chosenBrother->keys.end());
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

    /**
     * @brief Busca uma chave na árvore B.
     * @return Ponteiro para o nó que contém a chave, ou nullptr se a chave não foi encontrada.
     */
    BTreeNode<T, ORDER>* searchAux(BTreeNode<T, ORDER>* currentNode, int key, BTreeNode<T, ORDER>>*& parent){
        /**
         * Se for nulo, então não temos nada para fazer.
         */
        */
        if(currentNode == nullptr){
            return nullptr;
        }

        int i = 0;
        /**
         * Percorreremos as chaves do pai atual para localizar a chave que queremos.
         */
        while(i < currentNode->size && key > currentNode->keys[i]){
            i++;
        }

        /**
         * Se a chave foi encontrada no atual nó, nós retornaremos.
         */
        if(i < currentNode->size && key == currentNode->keys[i]){
            return currentNode;
        }

        /**
         * Se for uma folha e não chegamos na chave, retornamos nada
        */        
        if(currentNode->leaf){
            return nullptr;
        }

       /**
        * Antes de descer, atualizamos o pai.
        */
        parent = currentNode;
        return searchAux(currentNode->children[i], key, parent);
    }

    BTreeNode<T, ORDER>* search(int key, BTreeNode<T, ORDER>>*& parent){
        BTreeNode<T, ORDER>* parent = nullptr;
        return searchAux(root, key, parent);
    }

    void remove(T key){
        //TODO
    }
  }
};

#endif
