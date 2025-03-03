#include <iostream>
#include "scontainer.h"

int main() {
    scontainer<int> container;

    container.insert(5);
    container.insert(3);
    container.insert(7);
    container.insert(1);

    std::cout << "sorted asc: ";
    for (auto it = container.begin(); it != container.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    std::cout << "sorted desc: ";
    for (auto rit = container.rbegin(); rit != container.rend(); ++rit) {
        std::cout << *rit << " ";
    }
    std::cout << std::endl;

    return 0;
}
