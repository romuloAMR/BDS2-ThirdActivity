#include <iostream>

#include "Templates/BTree.hpp"

int main() {
    BTree<int,2>* tree = new BTree<int,2>();
    int op;
    int value;
    bool run = true;
    while (run) {
        std::cout <<
        "---- Options: ----\n" <<
        "| 0 - Exit       |\n" <<
        "| 1 - Insert     |\n" <<
        "------------------\n" <<
        std::endl;

        std::cout << "Option: ";
        std::cin >> op;

        switch (op) {
            case 0:
                run = false;
                break;
            case 1:
                std::cout << "Value: ";
                std::cin >> value;
                tree->insert(value);
                std::cout << "\n" << std::endl;
                tree->printTree();
                std::cout << "\n" << std::endl;
                break;
            default:
                std::cout << "Invalid option!" << std::endl;
                break;
        }
    }
    
}