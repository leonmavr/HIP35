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
        /* Insert a number */
        virtual void Insert(double num) = 0;

        // stack-altering functions
        virtual void Rdn() = 0;
        virtual void Enter() = 0;
        virtual void LastX() = 0;
        virtual void Clx() = 0;
        virtual void Cls() = 0;
        virtual void Pi() = 0;
        virtual void Sto(std::size_t idx) = 0;
        virtual void Rcl(std::size_t idx) = 0;

        // insert a token (operation/operand) and return curent result
        virtual double Calculate(std::string operation) = 0;
        // perform calculation in from string parsing (non-interactive mode)
        virtual double CalculateFromString(std::string rpnExpression) = 0;
};

} /* namespace Rpn */

#endif /* BASE_HPP */
