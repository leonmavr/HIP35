#ifndef BACKEND_HPP
#define BACKEND_HPP 

#include "base.hpp"
#include "stack.hpp"
#include <string> // string
#include <memory> // unique_ptr
#include <cmath> // sin, cos, tan, log10, sqrt
#include <stdexcept> // runtime_error


namespace Rpn {
/**
* @brief Implements a reverse Polish notation (RPN) calculator.
*        The architecture more or less follows the basic architecture
*        of the HP35 calculator. It contains a stack of 4 registers;
*        X, Y, Z and T (T for top, X for bottom). Therefore when we store
*        a number, it's written to the X register. When we query data
*        from the stack, the first item to be removed is X. It also
*        contains a LASTX register which stores the last value of X
*        before a function button is pressed. Its purpose is to correct
*        mistakes. The calculator supports the following basic operations
*        blueprinted in the `Base` class:
*        -- swapXY
*        -- Peek
*        -- Insert
*        -- RND (Rotate down)
*        -- Enter
*        -- Calculate (calculator's function buttons)
*        More details in each method's documentation.
*
*        Reference:
*        [1] Enter: Reverse Polish Notation Made Easy by J. Dodin
*            https://literature.hpcalc.org/community/enter-en.pdf
*/
class Backend: Base {
public:
    Backend();
    Backend(const Backend& other) : stack_(std::make_unique<Stack>(*other.stack_)), do_shift_up_(other.do_shift_up_), lastx_(other.lastx_) {}
    ~Backend() {}
    /**
     * @brief Swaps values of registers X and Y.
     */
    virtual void SwapXY();
    /**
     * @brief Gets current value of register X.
     *
     * @return Value of register X.
     */
    virtual double Peek() const { return (*stack_)[IDX_REG_X]; }
    /**
     * @brief Insert a number in the stack. Shifts the stack up if a
     *        number is previously inserted.
     *
     * @param num Decimal number to insert.
     */
    virtual void Insert(double num);
    /**
     * @brief Circularly rotates the stack down, e.g.:
     *           before:     after:
     *        T->   4          1
     *        Z->   3          4
     *        Y->   2          3
     *        X->   1          2
     */
    virtual void Rdn();
    /**
     * @brief Emulate enter key; shift the stack up, discarding T
     *        register and then clone Y into X, e.g.:
     *           before:     after:
     *        T->   4          3
     *        Z->   3          2
     *        Y->   2          1
     *        X->   1          1
     */
    virtual void Enter();
    /**
     * @brief Emulate pressing a function key. Function keys operate
     *        either on the X register (1-operand key, e.g. LOG) or
     *        on the X and Y registers (2-operand keys, e.g. +).
     *        If an 1-op calculation is entered, use register X as
     *        input and output. If a 2-op calculation is entered, use
     *        reg. X, Y as inputs, write output to Y and shift down
     *        the stack. Example:
     *           before: (LOG)  after:  |  before:    (/)    after:
     *        T->   4             3     |     4
     *        Z->   3             2     |     3                4
     *        Y->   2             1     |     2                3
     *        X->   1             0     |    0.1               20
     *
     * @param operation What operation to perform. This can be one
     *                  of the keys of function_key_1op_ or 
     *                  function_key_2op_ - see `Base` class
     *
     * @return The calculation's result 
     */
    virtual double Calculate(std::string operation);
    /**
     * @brief Calculte a results of an expression written in reverse
     *        Polish notation (RPN). In RPN only records operations 
     *        and operands and each operation follows its operands.
     *        Example: 12 7 - gives 5
     *                 6 / 2 COS gives COS(6/2)
     *
     * @param rpnExpression A string that contains a RPN notation
     *                      expression. Operands and operations are
     *                      separated by space. Variables are not
     *                      allowed.
     *
     * @return The numberic results of the RPN. 
     */
    virtual double CalculateFromString(std::string rpnExpression);
    /* Overrides the << operator for the class, e.g.std::cout << <Instance>; */
    friend std::ostream& operator<<(std::ostream& os, const Backend& backend);

private:
    // owns the object - unique_ptr manages its lifetime and deallocation
    std::unique_ptr<Stack> stack_;
    /* LASTX register - stores the value of X before a function is executed */
    double lastx_;
    // shift up stack after an operation is executed (e.g. +, -, etc.)  
    bool do_shift_up_;
};


} /* namespace Rpn */

#endif /* BACKEND_HPP */
