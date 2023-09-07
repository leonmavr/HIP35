#include "stack.hpp"

void Rpn::Stack::ShiftUp() {
    for (int i = stack_.size() - 1; i > 0; --i)
        stack_[i] = stack_[i - 1];
    stack_[0] = 0.0;
}

void Rpn::Stack::ShiftDown() {
    for (int i = 0; i < stack_.size() - 1; ++i)
        stack_[i] = stack_[i + 1];
    stack_[stack_.size() - 1] = 0.0;
}
