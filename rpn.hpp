#ifndef RPN_HPP
#define RPN_HPP
#include <array>
#include <iostream>
#include <cmath>
#include <unordered_map> // unordered_map
#include <functional> // function
#include <string>
#include <memory> // unique_ptr

namespace Rpn {

enum {
    IDX_REG_X = 0,
    IDX_REG_Y,
    IDX_REG_Z,
    IDX_REG_T,
};

class Base {
public:
    Base() {}
    ~Base() {}
    virtual void SwapXY() = 0;
    virtual double Peek() const = 0;
    virtual void Insert(double num) = 0;
    virtual void Rdn() = 0;
    virtual void Enter() = 0;
    virtual double Calculate(std::string operation) = 0;
    virtual double CalculateFromString(std::string rpnExpression) = 0;
protected:
    // string to function dictionary for single and two-type functions
    // Define some example functions
    // Create an ordered map that maps strings to function lambdas
    std::unordered_map<std::string, std::function<double(double)>> function_key_1op_;
    std::unordered_map<std::string, std::function<double(double, double)>> function_key_2op_;
};

class Backend;

class Stack {
public:
    Stack() { Clear(); }
    ~Stack() {}

    void ShiftUp();
    void ShiftDown();
    void Clear() { stack_ = { 0., 0., 0., 0. }; }
    double writeX(double x) { stack_[IDX_REG_X] = x; return stack_[IDX_REG_X]; }
    // index getter operator
    double operator[] (double i) const { return stack_[i]; }
    // index setter operator
    double& operator[] (double i) { return stack_[i]; }
    unsigned size() const { return stack_.size(); }

protected:
    std::array<double, 4> stack_;
private:
    // Backend can access its protected and private members
    friend class Backend;
};

class Backend: Base {
public:
    Backend():
        stack_(std::make_unique<Stack>()),
        do_shift_up_(true)
    {
        // Populate the map with function lambdas
        // 1-operand operations supported by the calculator
        function_key_1op_["sin"] =  [](double x) -> double { return sin(x); };
        function_key_1op_["cos"] =  [](double x) -> double { return cos(x); };
        function_key_1op_["tan"] =  [](double x) -> double { return tan(x); };
        function_key_1op_["log"] =  [](double x) -> double { return log10(x); };
        function_key_1op_["sqrt"] = [](double x) -> double { return sqrt(x); };
        function_key_1op_["chs"] =  [](double x) -> double { return -x; };
        function_key_1op_["inv"] =  [](double x) -> double { return 1/x; };
        // 2-operand operations supported by the calculator
        function_key_2op_["+"] =    [](double x, double y) -> double { return x + y; };
        function_key_2op_["-"] =    [](double x, double y) -> double { return y - x; };
        function_key_2op_["*"] =    [](double x, double y) -> double { return x * y; };
        function_key_2op_["/"] =    [](double x, double y) -> double {
            if (std::fabs(x) < 1e-10)
                throw std::runtime_error("[FATAL]: Division by zero.\n");
            return y/x; };
        function_key_2op_["^"] =    [](double x, double y) -> double { return pow(y, x); };
    }
    Backend(const Backend& other) : stack_(std::make_unique<Stack>(*other.stack_)), do_shift_up_(other.do_shift_up_), lastx_(other.lastx_) {}
    ~Backend() {}
    virtual void SwapXY();
    virtual double Peek() const { return (*stack_)[IDX_REG_X]; }
    virtual void Insert(double num);
    virtual void Rdn();
    virtual void Enter();
    virtual double Calculate(std::string operation);
    virtual double CalculateFromString(std::string rpnExpression);
    friend std::ostream& operator<<(std::ostream& os, const Backend& backend);

private:
    // Move any private members you want to be accessible to Stack to here
    // owns the object - unique_ptr manages its lifetime and deallocation
    std::unique_ptr<Stack> stack_; // Use std::unique_ptr to manage Stack
    // LASTX register - stores the value of X before a function is executed
    double lastx_;
    // shift up stack after an operation is executed (e.g. +, -, etc.)  
    bool do_shift_up_;
};

} /* namespace Rpn */

#endif /* RPN_HPP */
