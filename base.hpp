#ifndef BASE_HPP
#define BASE_HPP 

#include <unordered_map> // unordered_map
#include <functional> // function
#include <string> // string
#include <utility> // pair
#include <vector> // vector 

namespace Rpn {

/**
 * @brief Blueprint of the functions an HP35-based RPN calculator
 *        must implement. Originally, the are found at the keypad
 *        of HP35:
 *        https://en.wikipedia.org/wiki/HP-35#/media/File:HP-35_Red_Dot.jpg
 */
class IBackend {
    public:
        IBackend() {}
        ~IBackend() {}
        virtual void SwapXY() = 0;
        /* Returns the values of two registers, e.g. X and Y */
        virtual std::pair<double, double> Peek() const = 0;
        virtual void Insert(double num) = 0;
        virtual void Rdn() = 0;
        virtual void Enter() = 0;
        virtual void LastX() = 0;
        virtual void Clx() = 0;
        virtual void Cls() = 0;
        virtual void Pi() = 0;
        virtual double Calculate(std::string operation) = 0;
        virtual double CalculateFromString(std::string rpnExpression) = 0;
    protected:
        /**
        * Maps a string that indicates a stack-altering operation into
        * its corresponding method. The method is defined in this class
        * and can be Rdn (rotate down), enter, etc.
        */
        std::unordered_map<std::string, std::function<void(void)>>
            function_key_0op_;
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
