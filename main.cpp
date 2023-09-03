#include "rpn.hpp"
#include <iostream>

int main() {
    auto stack = RpnStack();
    for (int i = 0; i < 4; i++)
        std::cout << stack.stack_[i] << std::endl;
    stack.shiftUp();
    stack.writeX(1);
    stack.shiftUp();
    stack.writeX(2);
    stack.shiftUp();
    stack.writeX(3);
    stack.shiftUp();
    stack.writeX(4);
    for (int i = 0; i < 4; i++)
        std::cout << stack.peek() << std::endl;
}
