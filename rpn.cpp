#include "rpn.hpp"
#include <iostream> // ostream
#include <iomanip> // setprecision
#include <vector> // setprecision
#include <sstream> // istringstream
#include <stdexcept> // runtime_error 


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
    auto stackPtr = *stack_;
    auto oldFirst = stackPtr[0];
    for (int i = 0; i < stackPtr.size()-1; ++i)
        stackPtr[i] = stackPtr[i+1];
    stackPtr[stackPtr.size()-1] = oldFirst;
}

void RpnBackend::swapXY() {
    std::swap((*stack_)[IDX_REG_X], (*stack_)[IDX_REG_Y]);
}

void RpnBackend::insert(double num) {
    if (do_shift_up_)
        stack_->shiftUp();
    stack_->writeX(num);
}


void RpnBackend::enter() {
    stack_->shiftUp();
    (*stack_)[IDX_REG_X] = (*stack_)[IDX_REG_Y];
    do_shift_up_ = false;
}

static std::string toLowercase(const std::string& input) {
    std::string ret = input;
    for (char& c : ret)
        c = std::tolower(static_cast<unsigned char>(c));
    return ret;
}


double RpnBackend::calculate(std::string operation) {
    // Shift up the stack next time a number is inserted
    do_shift_up_ = true;
    // We did an operation so calculator needs to store register X
    // before the operation in register LASTX
    lastx_ = (*stack_)[IDX_REG_X];
    auto& registerX = (*stack_)[IDX_REG_X];
    auto& registerY = (*stack_)[IDX_REG_Y];
    operation = toLowercase(operation);
    bool validOperation = false;

    if (function_key_1op_.find(operation) != function_key_1op_.end()) {
        // query single operand op/s such as sin, log, etc.
        registerX  = function_key_1op_[operation](registerX);
        validOperation = true;
    }
    if (function_key_2op_.find(operation) != function_key_2op_.end()) {
        // query 2-op operations such as +, /, etc.
        registerY = function_key_2op_[operation](registerX,
                                                 registerY);
        stack_->shiftDown();
        validOperation = true;
    }
    if (validOperation)
    {
        return registerX;
    } else {
        throw std::runtime_error(std::string("[FATAL]: Invalid operation ") +
                                             operation + std::string("\n"));
    }
}

double RpnBackend::calculateFromString(std::string rpnExpression) {
    // split input by spaces
    std::istringstream iss(rpnExpression);
    // space-separated substrings
    std::vector<std::string> substrings;
    // current substring
    std::string token;

    while (iss >> token)
        substrings.push_back(token);

    bool previousTokenIsDigit = false;
    for (const std::string& substring : substrings) {
        // if substring not in dictionary keys, it's a digit so enter it
        // if substring is a digit and previous substring is a digit, press enter before entering it
        // std::stood
        auto it1 = function_key_1op_.find(toLowercase(substring));
        auto it2 = function_key_2op_.find(toLowercase(substring));
        if (it1 != function_key_1op_.end() || it2 != function_key_2op_.end()) {
            // function key (1-op or 2-op) pressed
            calculate(substring);
            previousTokenIsDigit = false;
        } else {
            // number was entered
            if (previousTokenIsDigit) {
                // press enter to separate the numbers
                enter();
            } 
            try {
                insert(std::stod(substring));
            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid input: " + substring + " at "
                   << e.what() << std::endl;
            }
            previousTokenIsDigit = true;
        }
    }        
    return (*stack_)[IDX_REG_X];
}

std::ostream& operator<<(std::ostream& os, const RpnBackend& backend) {
    const auto& stack = *(backend.stack_); // Dereference the unique_ptr to get the RpnStack

    os << std::fixed << std::setprecision(2) <<
        "X\tY\tZ\tT\tLASTX" << std::endl <<
        stack[IDX_REG_X] << "\t" <<
        stack[IDX_REG_Y] << "\t" <<
        stack[IDX_REG_Z] << "\t" <<
        stack[IDX_REG_T] << "\t" <<
        backend.lastx_ << std::endl;
    return os;
}
