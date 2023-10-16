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
    keypad_(keypad),
    sto_regs_({0})
{ 
    // map functions that can be input in non-interactive mode into
    // single key presses that the calculator accepts, e.g. LN -> l
    for (const auto& pair: keypad_.stack_keys)
        reverse_keys_[std::get<3>(pair.second)] = pair.first;
    for (const auto& pair: keypad_.single_arg_keys)
        reverse_keys_[std::get<3>(pair.second)] = pair.first;
    for (const auto& pair: keypad_.double_arg_keys)
        reverse_keys_[std::get<3>(pair.second)] = pair.first;
    for (const auto& pair: keypad_.storage_keys)
        reverse_keys_[std::get<3>(pair.second)] = pair.first;
}

void Rpn::Backend::Rdn() {
    // we always use the stack pointer because Stack class implements a [] operator
    auto old_first = (*stack_)[0];
    for (std::size_t i = 0; i < (*stack_).size() - 1; ++i)
        (*stack_)[i] = (*stack_)[i+1];
    (*stack_)[(*stack_).size() - 1] = old_first;
    // inform the observer
    NotifyValue(Peek());
    NotifyOperation(Key::kKeyRdn);
}

void Rpn::Backend::SwapXY() {
    std::swap((*stack_)[IDX_REG_X], (*stack_)[IDX_REG_Y]);
    // inform the observer
    NotifyValue(Peek());
    NotifyOperation(Key::kKeySwap);
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
    NotifyOperation(Key::kKeyEnter);
}

void Rpn::Backend::LastX() {
    // Make space to insert regisrer LASTX
    stack_->ShiftUp();
    (*stack_)[IDX_REG_X] = lastx_;
    // inform the observer
    NotifyValue(Peek());
    NotifyOperation(Key::kKeyLastX);
}

double Rpn::Backend::Calculate(std::string operation) {
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
    NotifyOperation(Key::kKeyClx); 
    NotifyValue(Peek()); 
}

void Rpn::Backend::Cls() {
    stack_->writeX(0.0);
    Enter();
    Enter();
    Enter();
    NotifyOperation(Key::kKeyCls); 
    NotifyValue(Peek()); 
}

void Rpn::Backend::Pi() {
    Insert(M_PI);
    // inform the observer 
    NotifyOperation(Key::kKeyPi); 
    NotifyValue(Peek()); 
}

void Rpn::Backend::Sto(std::size_t idx) {
    // silently ignore index errors
    if (idx < sto_regs_.size())
        sto_regs_[idx] = (*stack_)[IDX_REG_X];
    // after store, shift up to make space for new entries
    do_shift_up_ = true;
    NotifyOperation(Key::kKeyStore); 
    // doesn't change the stack so no values sent to observer
}

void Rpn::Backend::Rcl(std::size_t idx) {
    // silently ignore index errors
    if (idx < sto_regs_.size())
        (*stack_)[IDX_REG_X] = sto_regs_[idx];
    // after store, shift up to make space for new entries
    do_shift_up_ = true;
    NotifyOperation(Key::kKeyRcl); 
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

    for (std::string& substring : substrings) {
        // convert to upper
        std::transform(substring.begin(), substring.end(), substring.begin(),
                       [](unsigned char c) { return std::toupper(c); });

        const auto keypress = reverse_keys_[substring];
        // what type the last token was, e.g. number, operation, etc.
        auto token_type = kTypeNone;
        
        // if substring not in dictionary keys, it's a digit so enter it
        // if substring is a digit and previous substring is a digit, press enter before entering it
        const auto it1 = Key::keypad.stack_keys.find(keypress);
        const auto it2 = Key::keypad.single_arg_keys.find(keypress);
        const auto it3 = Key::keypad.double_arg_keys.find(keypress);
        const bool is_stack_op = it1 != Key::keypad.stack_keys.end();
        const bool is_numeric_op =
            (it2 != Key::keypad.single_arg_keys.end()) ||
            (it3 != Key::keypad.double_arg_keys.end());
        if (is_numeric_op) {
            // function key (1-op or 2-op) pressed; do the calculation
            Calculate(keypress);
            token_type = kTypeNumeric;
        } else if (is_stack_op) {
            const auto it = keypad_.stack_keys.find(keypress);
            std::get<0>(it->second)(*this); // call the stakc method
            token_type = kTypeStack;
        } else {
            // number was entered
            if (token_type == kTypeOperand) {
                // press enter to separate the numbers
                Enter();
            }
            try {
                // negative number tokens behin with ~ so replace it
                // with - before inserting it
                if (substring[0] == '~')
                    substring = "-" + substring.substr(1);
                Insert(std::stod(substring));
            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid input: " + substring + " at "
                    << e.what() << std::endl;
            }
        }
        token_type = kTypeOperand;
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
