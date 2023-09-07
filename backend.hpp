#ifndef BACKEND_HPP
#define BACKEND_HPP 

#include "base.hpp"
#include "stack.hpp"
#include <string> // string
#include <memory> // unique_ptr
#include <cmath> // sin, cos, tan, log10, sqrt
#include <stdexcept> // runtime_error


namespace Rpn {
class Backend: Base {
public:
    Backend();
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
    // owns the object - unique_ptr manages its lifetime and deallocation
    std::unique_ptr<Stack> stack_; // Use std::unique_ptr to manage Stack
    // LASTX register - stores the value of X before a function is executed
    double lastx_;
    // shift up stack after an operation is executed (e.g. +, -, etc.)  
    bool do_shift_up_;
};


} /* namespace Rpn */

#endif /* BACKEND_HPP */
