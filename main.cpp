#include "stack.hpp"
#include "backend.hpp"
#include <iostream>

int main() {
    auto obs1 = Observer();
    auto rpn = std::make_unique<Rpn::Backend>();
    // 2 5 * 4 + 3 2 * 1 + /
    rpn->Attach(&obs1);
#if 1
    rpn->Insert(2);
    rpn->Enter();
    rpn->Insert(5);
    rpn->Calculate("*");
    rpn->Insert(4);
    rpn->Calculate("+");
    std::cout << "should be + " << std::get<1>(obs1.GetState()) << std::endl;
    rpn->Insert(3);
    rpn->Enter();
    rpn->Insert(2);
    rpn->Calculate("*");
    rpn->Insert(1);
    rpn->Calculate("+");
    std::cout << rpn->Calculate("/") << std::endl; // 2
#endif
    std::cout << "---" << rpn->CalculateFromString("2 5 * 4 + 3 2 * 1 + /") << std::endl;
    //std::cout << "---" << rpn.calculateFromString("2 1.4 /") << std::endl;
    std::cout << *rpn << std::endl;
}
