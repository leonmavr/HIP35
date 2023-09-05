#include "rpn.hpp"
#include <iostream> // ostream
#include <iomanip> // setprecision
#include <vector> // setprecision
#include <sstream> // istringstream


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
    if (function_key_1op_.find(operation) != function_key_1op_.end()) {
        stack_.writeX(function_key_1op_[toLowercase(operation)](stack_[IDX_REG_X]));
        std::cout << "op = " << operation << std::endl;
        return stack_[IDX_REG_X];
    }
    if (function_key_2op_.find(operation) != function_key_2op_.end()) {
        std::cout << "op = " << operation << std::endl;
        stack_[IDX_REG_Y] = (function_key_2op_[toLowercase(operation)](stack_[IDX_REG_X],
                                                          stack_[IDX_REG_Y]));
        stack_.shiftDown();
        return stack_[IDX_REG_X];
    }
    // TODO:
    // throw exception
}

double RpnBackend::calculateFromString(std::string rpnExpression) {
    // split input by spaces
    std::istringstream iss(rpnExpression);
    std::vector<std::string> substrings;
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
            // found 1-op or 2-op operation
            calculate(substring);
            previousTokenIsDigit = false;
        } else {
            // decimal number
            if (previousTokenIsDigit) {
                // press enter
                enter();
            } 
#if 0
 try {
        myDouble = std::stod(str);
        std::cout << "Converted double: " << myDouble << std::endl;
    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid argument: " << e.what() << std::endl;
    } catch (const std::out_of_range& e) {
        std::cerr << "Out of range: " << e.what() << std::endl;
    }
#endif
            insert(std::stod(substring));
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
