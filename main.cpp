#include "rpn.hpp"
#include <iostream>

int main() {
    auto rpn = RpnBackend();
    // 2 5 * 4 + 3 2 * 1 + /
    rpn.insert(2);
    rpn.print();
    rpn.enter();
    rpn.print();
    rpn.insert(5);
    rpn.print();
    rpn.calculate("*");
    rpn.print();
    rpn.insert(4);
    rpn.print();
    rpn.calculate("+");
    rpn.print();
    rpn.insert(3);
    rpn.print();
    rpn.enter();
    rpn.print();
    rpn.insert(2);
    rpn.print();
    rpn.calculate("*");
    rpn.print();
    rpn.insert(1);
    rpn.print();
    rpn.calculate("+");
    rpn.print();
    rpn.calculate("/");
    rpn.print();
}
