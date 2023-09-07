#ifndef STACK_HPP
#define STACK_HPP 

#include <array>

namespace Rpn {

/**
* @brief Indexes for the 4 register of HP35's stack.
*/
enum {
    IDX_REG_X = 0,
    IDX_REG_Y,
    IDX_REG_Z,
    IDX_REG_T,
};

/**
 * Forward declaration; it will be a friend i.e. able to access
 * private/protected members of the stack.
 */
class Backend;

/**
 * @brief Implements the stack-based memory of an HP35 reverse Polish
 *        calculator [1]. The stack implemented is a LIFO (Last-In-
 *        First-Out) structure that consists of 4 contiguous cells.
 *        Each cell is referenced by an individual register for quick
 *        access. The top cells is accessed by the T register, then
 *        Z, Y, X registers all the way to the bottom (X). When data
 *        are written, the first register where they're inserted or
 *        stored is X. It differs from the traditional stack that
 *        programming languages use in the manner that it supports the
 *        following intrinsic operations:
 *        -- ShiftUp
 *        -- ShiftDown
 *        -- Clear
 *        -- WriteX
 *
 *        References:
 *        -----------
 *        [1] "Enter: Reverse Polish Notation Made Easy" by J. Dodin
 *            https://literature.hpcalc.org/community/enter-en.pdf
 */
class Stack {
    public:
        Stack() { Clear(); }
        ~Stack() {}

        /**
         * @brief  Shifts up the data in the stack by one position,
         *         discarding the T register. In the example below,
         *         empty cell means "don't care", however it is set
         *         to 0 in this implementation.
         *           before:     after:
         *        T->   4          3
         *        Z->   3          2
         *        Y->   2          1
         *        X->   1          
         */
        void ShiftUp();
        /**
         * @brief  Similar to `ShiftUp`, however shifts down the stack
         *         to discard the data in the X register.
         *             before:     after:
         *          T->   4          
         *          Z->   3          4
         *          Y->   2          3
         *          X->   1          2 
         */
        void ShiftDown();
        void Clear() { stack_ = { 0., 0., 0., 0. }; }
        double writeX(double x) { stack_[IDX_REG_X] = x; return stack_[IDX_REG_X]; }
        /* index getter operator */
        double operator[] (double i) const { return stack_[i]; }
        /* index setter operator */
        double& operator[] (double i) { return stack_[i]; }
        unsigned size() const { return stack_.size(); }

    protected:
        std::array<double, 4> stack_;
    private:
        // Backend can access its protected and private members
        friend class Backend;
};

} /* namespace Rpn */

#endif /* STACK_HPP */
