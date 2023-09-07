#ifndef BASE_HPP
#define BASE_HPP 

#include <unordered_map> // unordered_map
#include <functional> // function
#include <string> // string

namespace Rpn {

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
        /**
        * Maps a string that indicates the calculator's operation to
        * a function. A function is what the calculator shall perform
        * after the corresponding operation (string) is pressed. The 
        * operation takes either the register X (e.g. sin(X)) or the
        * registers X, Y (e.g. X+Y) as operands.
        */
        std::unordered_map<std::string, std::function<double(double)>>
            function_key_1op_;
        std::unordered_map<std::string, std::function<double(double, double)>>
            function_key_2op_;
};

} /* namespace Rpn */

#endif /* BASE_HPP */
