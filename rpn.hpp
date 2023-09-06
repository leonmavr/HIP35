#ifndef RPN_HPP
#define RPN_HPP
#include <array>
#include <iostream>
#include <cmath>
#include <unordered_map>
#include <functional>
#include <string>

enum {
    IDX_REG_X = 0,
    IDX_REG_Y,
    IDX_REG_Z,
    IDX_REG_T,
};

class RpnBase {
public:
    RpnBase() { }
    virtual void swapXY() = 0;
    virtual double peek() const = 0;
    virtual void insert(double num) = 0;
    virtual void rdn() = 0;
    virtual void enter() = 0;
    virtual double calculate(std::string operation) = 0;
    virtual double calculateFromString(std::string rpnExpression) = 0;
protected:
    // string to function dictionary for single and two-type functions
    // Define some example functions
    // Create an ordered map that maps strings to function lambdas
    std::unordered_map<std::string, std::function<double(double)>> function_key_1op_;
    std::unordered_map<std::string, std::function<double(double, double)>> function_key_2op_;
};

class RpnBackend;

class RpnStack {
public:
    RpnStack() { clear(); }
    ~RpnStack() {}

    void shiftUp();
    void shiftDown();
    void clear() { stack_ = { 0., 0., 0., 0. }; }
    double writeX(double x) { stack_[IDX_REG_X] = x; return stack_[IDX_REG_X]; }
    // index getter operator
    double operator[] (double i) const { return stack_[i]; }
    // index setter operator
    double& operator[] (double i) { return stack_[i]; }
    unsigned size() const { return stack_.size(); }

protected:
    std::array<double, 4> stack_;
private:
    friend class RpnBackend;
};

class RpnBackend: RpnBase {
public:
    RpnBackend():
        do_shift_up_(true)
    {
        // Populate the map with function lambdas
        function_key_1op_["sin"] =  [](double x) -> double { return sin(x); };
        function_key_1op_["cos"] =  [](double x) -> double { return cos(x); };
        function_key_1op_["tan"] =  [](double x) -> double { return tan(x); };
        function_key_1op_["log"] =  [](double x) -> double { return log10(x); };
        function_key_1op_["sqrt"] = [](double x) -> double { return sqrt(x); };
        function_key_1op_["chs"] =  [](double x) -> double { return -x; };
        function_key_1op_["inv"] =  [](double x) -> double { return 1/x; };
        function_key_2op_["+"] =    [](double x, double y) -> double { return x+y; };
        function_key_2op_["-"] =    [](double x, double y) -> double { return y-x; };
        function_key_2op_["*"] =    [](double x, double y) -> double { return x*y; };
        function_key_2op_["/"] =    [](double x, double y) -> double {
            if (std::fabs(x) < 1e-10)
                throw std::runtime_error("[FATAL]: Division by zero.\n");
            return y/x; };
        function_key_2op_["^"] =    [](double x, double y) -> double { return pow(y,x); };
    }
    ~RpnBackend() {}
    // Make RpnStack a friend class
    //friend class RpnStack;
    virtual void swapXY() { std::swap(stack_[IDX_REG_X], stack_[IDX_REG_Y]); }
    virtual double peek() const { std::cout << " " << stack_[IDX_REG_X] << ", " << stack_[IDX_REG_Y] << std::endl;;return stack_[IDX_REG_X]; }
    virtual void insert(double num);
    virtual void rdn();
    virtual void enter();
    virtual double calculate(std::string operation);
    virtual double calculateFromString(std::string rpnExpression);
    friend std::ostream& operator<<(std::ostream& os, const RpnBackend& backend);

private:
    // Move any private members you want to be accessible to RpnStack to here
    RpnStack stack_;
    // LASTX register - stores the value of X before a function is executed
    double lastx_;
    // shift up stack after an operation is executed (e.g. +, -, etc.)  
    bool do_shift_up_;
};

#endif

