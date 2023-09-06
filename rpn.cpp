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

static std::string toLowercase(const std::string& input) {
    std::string ret = input;
    for (char& c : ret)
        c = std::tolower(static_cast<unsigned char>(c)); // Convert character to lowercase
    return ret;
}


double RpnBackend::calculate(std::string operation) {
    do_shift_up_ = true;
    // TODO: copy to LASTX register
    lastx_ = stack_[IDX_REG_X];
    operation = toLowercase(operation);
    bool validOperation = false;
    if (function_key_1op_.find(operation) != function_key_1op_.end()) {
        // query single operand op/s such as sin, log, etc.
        stack_[IDX_REG_X] = function_key_1op_[operation](stack_[IDX_REG_X]);
        validOperation = true;
    }
    if (function_key_2op_.find(operation) != function_key_2op_.end()) {
        // query 2-op operations such as +, /, etc.
        stack_[IDX_REG_Y] = (function_key_2op_[operation](stack_[IDX_REG_X],
                                                          stack_[IDX_REG_Y]));
        stack_.shiftDown();
        validOperation = true;
    }
    if (validOperation)
    {
        return stack_[IDX_REG_X];
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
    return stack_[IDX_REG_X];
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
