#include <iostream>

#include "Templates/BTree.hpp"

int main() {
    BTree<int,2>* tree = new BTree<int,2>();
    std::cout << tree << std::endl;
    return 0;
}