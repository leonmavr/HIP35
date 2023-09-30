#include "observer.hpp"
#include "backend.hpp"
#include "stack.hpp"
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

Rpn::Backend::Backend():
    stack_(std::make_unique<Stack>()),
    do_shift_up_(true)
{
    // stack-altering operations supported by the calculator
    function_key_0op_["rdn"] =   [this](void) -> void { Rdn(); };
    function_key_0op_["lastx"] = [this](void) -> void { LastX(); };
    function_key_0op_["swap"] =  [this](void) -> void { SwapXY(); };
    function_key_0op_["enter"] = [this](void) -> void { Enter(); };
    function_key_0op_["pi"] =    [this](void) -> void { Pi(); };
    function_key_0op_["clr"] =   [this](void) -> void { Clr(); };
    // 1-operand numerical operations
    function_key_1op_["chs"] =   [](double x) -> double { return -x; };
    function_key_1op_["inv"] =   [](double x) -> double { return 1/x; };
    function_key_1op_["sin"] =   [](double x) -> double { return sin(x); };
    function_key_1op_["cos"] =   [](double x) -> double { return cos(x); };
    function_key_1op_["tan"] =   [](double x) -> double { return tan(x); };
    function_key_1op_["exp"] =   [](double x) -> double { return exp(x); };
    function_key_1op_["ln"] =    [](double x) -> double { return log(x); };
    function_key_1op_["log"] =   [](double x) -> double { return log10(x); };
    function_key_1op_["sqrt"] =  [](double x) -> double { return sqrt(x); };
    // 2-operand operations
    function_key_2op_["+"] =     [](double x, double y) -> double
                                 { return x + y; };
    function_key_2op_["-"] =     [](double x, double y) -> double
                                 { return y - x; };
    function_key_2op_["*"] =     [](double x, double y) -> double
                                 { return x * y; };
    function_key_2op_["/"] =     [](double x, double y) -> double
    {
        if (std::fabs(x) < 1e-10)
            throw std::runtime_error("[FATAL]: Backend: Division by zero.\n");
        return y/x;
    };
    function_key_2op_["^"] =     [](double x, double y) -> double
        { return pow(y, x); };
}

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
    NotifyOperation("enter");
}

void Rpn::Backend::LastX() {
    (*stack_)[IDX_REG_X] = lastx_;
    // inform the observer
    NotifyValue(Peek());
    NotifyOperation("lastx");
}

static std::string ToLowercase(const std::string& input) {
    std::string ret = input;
    for (char& c : ret)
        c = std::tolower(static_cast<unsigned char>(c));
    return ret;
}


double Rpn::Backend::Calculate(std::string operation) {
    // Shift up the stack next time a number is inserted
    do_shift_up_ = true;
    // We did an operation so calculator needs to store register X
    // before the operation in register LASTX
    lastx_ = (*stack_)[IDX_REG_X];
    auto& registerX = (*stack_)[IDX_REG_X];
    auto& registerY = (*stack_)[IDX_REG_Y];
    bool valid_operation = false;
    // because maps always take lowercase keys
    operation = ToLowercase(operation);

    if (function_key_1op_.find(operation) != function_key_1op_.end()) {
        // query single operand op/s such as sin, log, etc.
        registerX  = function_key_1op_[operation](registerX);
        valid_operation = true;
    }
    if (function_key_2op_.find(operation) != function_key_2op_.end()) {
        // query 2-operant operations such as +, /, etc.
        registerY = function_key_2op_[operation](registerX,
                                                registerY);
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

void Rpn::Backend::Clr() {
    // If an operation was just made, shift down first to erase register Y
    if (do_shift_up_)
        Rdn();
    Insert(0);
    Rdn();
    Insert(0);
    Rdn();
    Insert(0);
    Rdn();
    Insert(0);
    // inform the observer 
    NotifyOperation("clr"); 
    NotifyValue(Peek()); 
}

void Rpn::Backend::Pi() {
    (*stack_)[IDX_REG_X] = M_PI;
    // inform the observer 
    NotifyOperation("pi"); 
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
        auto it1 = function_key_1op_.find(ToLowercase(substring));
        auto it2 = function_key_2op_.find(ToLowercase(substring));
        if (it1 != function_key_1op_.end() || it2 != function_key_2op_.end()) {
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

void Rpn::Backend::DoStackOperation(const std::string& operation) {
    auto it = function_key_0op_.find(ToLowercase(operation));
    if (it != function_key_0op_.end())
        function_key_0op_[ToLowercase(operation)]();
}

bool Rpn::Backend::IsInStackOperations(const std::string& string) const {
    auto it = function_key_0op_.find(ToLowercase(string));
    if (it != function_key_0op_.end())
        return true;
    return false;
}

bool Rpn::Backend::IsInFunctions(const std::string& string) const {
    auto it1 = function_key_1op_.find(ToLowercase(string));
    if (it1 != function_key_1op_.end())
        return true;
    auto it2 = function_key_2op_.find(ToLowercase(string));
    if (it2 != function_key_2op_.end())
        return true;
    return false;
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
