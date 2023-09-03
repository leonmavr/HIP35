#ifndef RPN_HPP
#define RPN_HPP 

#include <array>

enum {
    IDX_REG_X = 0,
    IDX_REG_Y,
    IDX_REG_Z,
    IDX_REG_T,
};

class RpnStack {
    public:
        RpnStack() { stack_ = {0., 0., 0., 0.}; }; 
        ~RpnStack() { }; 
        double peek() const { return stack_[IDX_REG_X]; };
        void swapXY() { std::swap(stack_[IDX_REG_X], stack_[IDX_REG_Y]); };
        void rdn();
        void writeX(double x) { stack_[IDX_REG_X] = x; };
        void shiftUp();
        void shiftDown();
        void clear() { stack_ = {0., 0., 0., 0.}; }
        std::array<double, 4> stack_;
    private:
};

class RpnBackend {
    public:
    private:

};


#endif /* RPN_HPP */
