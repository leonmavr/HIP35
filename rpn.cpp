#include "rpn.hpp"
#include <iostream> // ostream
#include <iomanip> // setprecision


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
    if (do_shift_up_)
        stack_.shiftUp();
    stack_.writeX(num);
}


void RpnBackend::enter() {
    stack_.shiftUp();
    stack_[IDX_REG_X] = stack_[IDX_REG_Y];
    do_shift_up_ = false;
}


double RpnBackend::calculate(std::string operation) {
    do_shift_up_ = true;
    // TODO: copy to LASTX register
    lastx_ = stack_[IDX_REG_X];
    if (function_key_1op_.find(operation) != function_key_1op_.end()) {
        stack_.writeX(function_key_1op_[operation](stack_[IDX_REG_X]));
        return stack_[IDX_REG_X];
    }
    if (function_key_2op_.find(operation) != function_key_2op_.end()) {
        stack_[IDX_REG_Y] = (function_key_2op_[operation](stack_[IDX_REG_X],
                                                          stack_[IDX_REG_Y]));
        stack_.shiftDown();
        return stack_[IDX_REG_X];
    }
    // TODO:
    // throw exception
}

std::ostream& operator<<(std::ostream& os, const RpnBackend& backend) {
    os << std::setprecision(2) << "X\tY\tZ\tT\tLASTX" << std::endl <<
        backend.stack_[IDX_REG_X] << "\t" <<
        backend.stack_[IDX_REG_Y] << "\t" <<
        backend.stack_[IDX_REG_Z] << "\t" <<
        backend.stack_[IDX_REG_T] << "\t" <<
        backend.lastx_ << std::endl;
    return os;
}
