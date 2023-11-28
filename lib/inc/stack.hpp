#ifndef STACK_HPP
#define STACK_HPP 

#include <array>

namespace backend {

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
         *           before:     after: | before:           after:
         *        T->   4          3    |    t   +----------> T
         *        Z->   3          2    |    z --+  +-------> Z
         *        Y->   2          1    |    y -----+  +----> Y
         *        X->   1               |    x --------+      X = 0
         */
        void ShiftUp();
        /**
         * @brief  Shifts down ("drops" stack) all elements of the
         *         stack by one, discarding the value of X. The new
         *         black entry in T register is filled with its
         *         previous data [2], replicating T into Z and T.
         *             before:     after:  | before:     after:
         *          T->   4          4     |   t ------+-----> T
         *          Z->   3          4     |   z       +-----> Z
         *          Y->   2          3     |   y ------------> Y
         *          X->   1          2     |   x ------------> X
         *
         *          References:
         *          -----------
         *          [2] HP 35s scientific calculator user's guide 1st
         *              ed., RPN: The Automatic Memory Stack 2-7,
         *              p.53,
         *              http://h10032.www1.hp.com/ctg/Manual/c01579350
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

} /* namespace backend */

#endif /* STACK_HPP */
