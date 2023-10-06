#include "observer.hpp"
#include "backend.hpp"
#include "stack.hpp"
#include "keypad.hpp"
#include <iostream> // ostream
#include <iomanip> // setprecision, fixed
#include <vector> // vector 
#include <sstream> // istringstream
#include <stdexcept> // runtime_error 
#include <algorithm> // erase, remove
#include <cmath> // M_PI


void Subject::Detach(Observer* observer) {
    observers_.erase(
        std::remove(observers_.begin(), observers_.end(), observer),
        observers_.end()
    );
} 

void Subject::NotifyValue(std::pair<double, double> registers) {
    for (const auto& observer : observers_)
        observer->UpdateRegisters(registers);
}

void Subject::NotifyOperation(const std::string& operation) {
    for (const auto& observer : observers_)
        observer->UpdateOperation(operation);
}

Rpn::Backend::Backend(const Key::Keypad& keypad):
    stack_(std::make_unique<Stack>()),
    do_shift_up_(true),
    keypad_(keypad)
{ }

void Rpn::Backend::Rdn() {
    // we always use the stack pointer because Stack class implements a [] operator
    auto old_first = (*stack_)[0];
    for (std::size_t i = 0; i < (*stack_).size() - 1; ++i)
        (*stack_)[i] = (*stack_)[i+1];
    (*stack_)[(*stack_).size() - 1] = old_first;
    // inform the observer
    NotifyValue(Peek());
    NotifyOperation("rdn");
}

void Rpn::Backend::SwapXY() {
    std::swap((*stack_)[IDX_REG_X], (*stack_)[IDX_REG_Y]);
    // inform the observer
    NotifyValue(Peek());
    NotifyOperation("swap");
}

void Rpn::Backend::Insert(double num) {
    if (do_shift_up_)
        stack_->ShiftUp();
    stack_->writeX(num);
    // notify class observers about new value
    NotifyValue(Peek());
}


void Rpn::Backend::Enter() {
    stack_->ShiftUp();
    (*stack_)[IDX_REG_X] = (*stack_)[IDX_REG_Y];
    do_shift_up_ = false;
    // notify class observer since enter manipulates the stack
    NotifyValue(Peek());
    // don't forget to notify the observer so we can use the event later
    // TODO: address KEY_ENTER
    NotifyOperation("enter");
}

void Rpn::Backend::LastX() {
    // Make space to insert regisrer LASTX
    stack_->ShiftUp();
    (*stack_)[IDX_REG_X] = lastx_;
    // inform the observer
    NotifyValue(Peek());
    NotifyOperation(KEY_LASTX);
}

double Rpn::Backend::Calculate(std::string operation) {
    //std::cout << "----------- " << operation << "-------\n";
    // Shift up the stack next time a number is inserted
    do_shift_up_ = true;
    auto& registerX = (*stack_)[IDX_REG_X];
    auto& registerY = (*stack_)[IDX_REG_Y];
    // We did an operation so calculator needs to store register X
    // before the operation in register LASTX
    lastx_ = registerX;
    bool valid_operation = false;
    // because maps always take lowercase keys
    //operation = ToLowercase(operation);

    auto it1 = keypad_.single_arg_keys.find(operation);
    if (it1 != keypad_.single_arg_keys.end()) {
        // query single operand op/s such as sin, log, etc.
        //registerX  = function_key_1op_[operation](registerX);
        registerX = std::get<0>(it1->second)(registerX);
        valid_operation = true;
    }
    //std::cout << "------------- " << operation << "----------\n";
    auto it2 = keypad_.double_arg_keys.find(operation);
    if (it2 != keypad_.double_arg_keys.end()) {
        // query 2-operant operations such as +, /, etc.
        registerY = std::get<0>(it2->second)(registerX, registerY);
        // drop old register X
        stack_->ShiftDown();
        valid_operation = true;
    }
    if (valid_operation)
    {
        // Notify observers about the new operation and value
        NotifyOperation(operation); 
        NotifyValue(Peek()); 
        return registerX;
    } else {
        throw std::runtime_error(std::string("[FATAL]: Invalid operation ") +
                                            operation + std::string("\n"));
    }
}

void Rpn::Backend::Clx() {
    stack_->writeX(0.0);
    // inform the observer 
    NotifyOperation(KEY_CLX); 
    NotifyValue(Peek()); 
}

void Rpn::Backend::Cls() {
    stack_->writeX(0.0);
    Enter();
    Enter();
    Enter();
    NotifyOperation(KEY_CLS); 
    NotifyValue(Peek()); 
}

void Rpn::Backend::Pi() {
    Insert(M_PI);
    // inform the observer 
    NotifyOperation(KEY_PI); 
    NotifyValue(Peek()); 
}

double Rpn::Backend::CalculateFromString(std::string rpnExpression) {
    // split input by spaces
    std::istringstream iss(rpnExpression);
    // space-separated substrings
    std::vector<std::string> substrings;
    // current substring (i.e. operation or data)
    std::string token;

    while (iss >> token)
        substrings.push_back(token);

    bool previous_token_is_digit = false;
    for (const std::string& substring : substrings) {
        // if substring not in dictionary keys, it's a digit so enter it
        // if substring is a digit and previous substring is a digit, press enter before entering it
        auto it1 = Key::keypad.single_arg_keys.find(substring);
        auto it2 = Key::keypad.double_arg_keys.find(substring);
        if (it1 != Key::keypad.single_arg_keys.end() || it2 != Key::keypad.double_arg_keys.end()) {
            // function key (1-op or 2-op) pressed; do the calculation
            Calculate(substring);
            previous_token_is_digit = false;
        } else {
            // number was entered
            if (previous_token_is_digit) {
                // press enter to separate the numbers
                Enter();
            }
            try {
                Insert(std::stod(substring));
            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid input: " + substring + " at "
                << e.what() << std::endl;
            }
            previous_token_is_digit = true;
        }
    }        
    return (*stack_)[IDX_REG_X];
}




namespace Rpn {
    std::ostream& operator<<(std::ostream& os, const Backend& backend) {
        const auto& stack = *(backend.stack_);
        os << std::fixed << std::setprecision(2) <<
            "X\tY\tZ\tT\tLASTX" << std::endl <<
            stack[IDX_REG_X] << "\t" <<
            stack[IDX_REG_Y] << "\t" <<
            stack[IDX_REG_Z] << "\t" <<
            stack[IDX_REG_T] << "\t" <<
            backend.lastx_ << std::endl;
        return os;
    }
} /* namespace Rpn */
