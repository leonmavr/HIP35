#include "stack.hpp"

void backend::Stack::ShiftUp() {
    for (std::size_t i = stack_.size() - 1; i > 0; --i)
        stack_[i] = stack_[i - 1];
    stack_[0] = 0.0;
}

void backend::Stack::ShiftDown() {
    // The old T register (top of the stack) will be replicated into
    // the new top
    auto old_top = stack_[stack_.size() - 1];
    for (std::size_t i = 0; i < stack_.size() - 1; ++i)
        stack_[i] = stack_[i + 1];
    // replicate old top - see reference in doc in .hpp file
    stack_[stack_.size() - 1] = old_top;
}
