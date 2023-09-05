#include "rpn.hpp"
#include <iostream>

int main() {
    auto rpn = RpnBackend();
    // 2 5 * 4 + 3 2 * 1 + /
    rpn.insert(2);
    rpn.enter();
    rpn.insert(5);
    rpn.calculate("*");
    rpn.insert(4);
    rpn.calculate("+");
    rpn.insert(3);
    rpn.enter();
    rpn.insert(2);
    rpn.calculate("*");
    std::cout << rpn;
    rpn.insert(1);
    rpn.calculate("+");
    std::cout << rpn.calculate("log") << std::endl; // 2
    std::cout << "---" << rpn.calculateFromString("2 5 * 4 + 3 2 * 1 + log") << std::endl;
    std::cout << rpn << std::endl;
}
