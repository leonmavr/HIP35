#ifndef RPN_HPP
#define RPN_HPP
#include <array>

enum {
    IDX_REG_X = 0,
    IDX_REG_Y,
    IDX_REG_Z,
    IDX_REG_T,
};

class RpnBase {
public:
    virtual void swapXY() = 0;
    virtual double peek() const = 0;
    virtual void insert(double num) = 0;
    virtual void rdn() = 0;
    virtual void enter() = 0;
};

class RpnBackend;

class RpnStack {
public:
    RpnStack() { clear(); }
    ~RpnStack() {}

    void shiftUp();
    void shiftDown();
    void clear() { stack_ = { 0., 0., 0., 0. }; }
    void writeX(double x) { stack_[IDX_REG_X] = x; }
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
        is_init_(false)
    {}
    ~RpnBackend() {}
    // Make RpnStack a friend class
    //friend class RpnStack;
    virtual void swapXY() { std::swap(stack_[IDX_REG_X], stack_[IDX_REG_Y]); }
    virtual double peek() const { return stack_[IDX_REG_X]; }
    virtual void insert(double num);
    virtual void rdn();
    virtual void enter();

private:
    // Move any private members you want to be accessible to RpnStack to here
    RpnStack stack_;
    bool is_init_;
};

#endif

