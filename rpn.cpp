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
    stack_[stack_.size()] = oldFirst;
}


void RpnBackend::insert(double num) {
    if (!is_init_) {
        is_init_ = true;
    } else {
        stack_.shiftUp();
    }
    stack_.writeX(num);
}


void RpnBackend::enter() {
    stack_.shiftUp();
    stack_[IDX_REG_X] = stack_[IDX_REG_Y];
}
