#include <iostream>

#include "Templates/BTree.hpp"


int main() {

    BTree<int, 2> tree; // Árvore B de ordem 2 (d = 2)

    // Inserção de chaves
    tree.insert(10);
    tree.insert(20);
    tree.insert(30);
    tree.insert(40);
    tree.insert(50);
    tree.insert(60);
    tree.insert(70);
    tree.insert(80);
   
    tree.insert(90);
    tree.insert(100);
    tree.insert(110);
    tree.insert(120);
    tree.insert(130);
    tree.insert(140);
    tree.insert(150);
    tree.insert(160);

    tree.insert(82);
    tree.insert(87);
    

    tree.generateDotFile("arvoreB.dot");

    std::cout << "Árvore após inserções:" << std::endl;
    tree.printTree();

    tree.remove(150);
    tree.generateDotFile("arvoreBREM.dot");

    tree.printTree();

    /*// Remoção de chaves
    tree.remove(10);
    tree.remove(20);
    tree.remove(30);
    tree.remove(40);
    tree.remove(50);
    tree.remove(60);

    tree.generateDotFile("arvoreBREM.dot");
    

    std::cout << "Árvore após remoções:" << std::endl;
    tree.printTree();*/

    return 0;

}