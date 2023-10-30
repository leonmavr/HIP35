#ifndef IBACKEND_HPP
#define IBACKEND_HPP 

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
        virtual ~IBackend() {}
        //------------------------------------------------------
        // Stack manipulation keys                         
        //-------------------------------------------------------
        /** @brief Abstract method for the `SWAP` key */
        virtual void SwapXY() = 0;
        /**
        * @brief Abstract method for insertion functionality.
        * @param num Number to insert
        */
        virtual void Insert(double num) = 0;
        /** @brief Abstract method for `RDN` (rotate down). */ 
        virtual void Rdn() = 0;
        /** @brief Abstract method for the `ENTER` key. */
        virtual void Enter() = 0;
        /** @brief Abstract method for the `LASTX` key. */
        virtual void LastX() = 0;
        /** @brief Abstract method for the `CLX` key. */
        virtual void Clx() = 0;
        /** @brief Abstract method for the `CLS` key. */
        virtual void Cls() = 0;
        /** @brief Abstract method for the `PI` key. */
        virtual void Pi() = 0;
        // TODO:
        // see http://h10032.www1.hp.com/ctg/Manual/c01579350 p32
        virtual void Eex(std::string x) = 0;

        //-------------------------------------------------------
        // Storage/load keys
        //-------------------------------------------------------
        /** @brief Abstract method for the `STO` key. */
        virtual void Sto(std::string name) = 0;
        /** @brief Abstract method for the `RCL` key. */
        virtual void Rcl(std::string name) = 0;

        //-------------------------------------------------------
        // Execution methods
        //-------------------------------------------------------
        /** @brief  Returns the values of two registers, e.g. X and Y */
        virtual std::pair<double, double> Peek() const = 0;
        /** @brief  Abstract method for calculating last token */
        virtual double Calculate(std::string operation) = 0;
        /** @brief  Abstract method for calculating from a given RPN string */
        virtual double CalculateFromString(std::string rpnExpression) = 0;
};

} /* namespace Rpn */

#endif /* IBACKEND_HPP */
