#ifndef STACK_HPP
#define STACK_HPP 

#include <array>

namespace Rpn {

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

} /* namespace Rpn */

#endif /* STACK_HPP */
