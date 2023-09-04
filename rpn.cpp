#include "rpn.hpp"


void RpnStack::shiftUp() {
    for (int i = stack_.size()-1; i > 0; --i)
        stack_[i] = stack_[i-1];
    stack_[0] = 0.0;
}

void RpnStack::shiftDown() {
    for (int i = 0; i < stack_.size()-1; ++i)
        stack_[i] = stack_[i+1];
    stack_[stack_.size()-1] = 0.0;
}

void RpnBackend::rdn() {
    auto oldFirst = stack_[0];
    for (int i = 0; i < stack_.size()-1; ++i)
        stack_[i] = stack_[i+1];
    stack_[stack_.size()-1] = oldFirst;
}


void RpnBackend::insert(double num) {
    // if pressed enter don't shift up else do
    if (!pressed_enter_) {
        stack_.shiftUp();
        pressed_enter_ = false;
    }
    stack_.writeX(num);
}


void RpnBackend::enter() {
    stack_.shiftUp();
    stack_[IDX_REG_X] = stack_[IDX_REG_Y];
    pressed_enter_ = true;
}

void RpnBackend::print() {
    for (int i = 0; i < stack_.size(); i++)
        std::cout << stack_[i] << ", ";
    std::cout << std::endl;
}

double RpnBackend::calculate(std::string operation) {
    pressed_enter_ = false;
    if (function_key_1op_.find(operation) != function_key_1op_.end()) {
        stack_.writeX(function_key_1op_[operation](stack_[IDX_REG_X]));
        return stack_[IDX_REG_X];
    }
    if (function_key_2op_.find(operation) != function_key_2op_.end()) {
        std::cout << "2op: ";
        print();
        stack_[IDX_REG_Y] = (function_key_2op_[operation](stack_[IDX_REG_X],
                                                          stack_[IDX_REG_Y]));
        stack_.shiftDown();
        return stack_[IDX_REG_X];
    }
    // TODO:
    // throw exception
}
