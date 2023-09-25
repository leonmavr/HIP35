#include "observer.hpp"
#include "backend.hpp"
#include "stack.hpp"
#include <iostream> // ostream
#include <iomanip> // setprecision, fixed
#include <vector> // vector 
#include <sstream> // istringstream
#include <stdexcept> // runtime_error 
#include <algorithm> // erase, remove


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
    // 1-operand operations supported by the calculator
    function_key_1op_["chs"] =  [](double x) -> double { return -x; };
    function_key_1op_["inv"] =  [](double x) -> double { return 1/x; };
    function_key_1op_["sin"] =  [](double x) -> double { return sin(x); };
    function_key_1op_["cos"] =  [](double x) -> double { return cos(x); };
    function_key_1op_["tan"] =  [](double x) -> double { return tan(x); };
    function_key_1op_["log"] =  [](double x) -> double { return log10(x); };
    function_key_1op_["sqrt"] = [](double x) -> double { return sqrt(x); };
    // 2-operand operations supported by the calculator
    function_key_2op_["+"] =    [](double x, double y) -> double
                                { return x + y; };
    function_key_2op_["-"] =    [](double x, double y) -> double
                                { return y - x; };
    function_key_2op_["*"] =    [](double x, double y) -> double
                                { return x * y; };
    function_key_2op_["/"] =    [](double x, double y) -> double
    {
        if (std::fabs(x) < 1e-10)
            throw std::runtime_error("[FATAL]: Division by zero.\n");
        return y/x;
    };
    function_key_2op_["^"] =    [](double x, double y) -> double
        { return pow(y, x); };
}
void Rpn::Backend::Rdn() {
    auto stackPtr = *stack_;
    auto old_first = stackPtr[0];
    for (int i = 0; i < stackPtr.size() - 1; ++i)
        stackPtr[i] = stackPtr[i+1];
    stackPtr[stackPtr.size() - 1] = old_first;
}

void Rpn::Backend::SwapXY() {
    std::swap((*stack_)[IDX_REG_X], (*stack_)[IDX_REG_Y]);
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
    // don't forget to notify the observer so we can use the event later
    NotifyOperation("enter");
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

bool Rpn::Backend::IsInFunctions(const std::string& string) const {
    auto it1 = function_key_1op_.find(ToLowercase(string));
    if (it1 != function_key_1op_.end())
        return true;
    auto it2 = function_key_2op_.find(ToLowercase(string));
    if (it2 != function_key_2op_.end())
        return true;
    return false;
}

std::vector<std::string> Rpn::Backend::GetFunctions() const {
    std::vector <std::string> ret;
    // Access the private unordered map and print its keys
    for (const auto& pair : function_key_1op_) 
        ret.push_back(pair.first);
    for (const auto& pair : function_key_2op_) 
        ret.push_back(pair.first);
    return ret;
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
