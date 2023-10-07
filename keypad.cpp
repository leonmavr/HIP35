#include "keypad.hpp" 
#include "backend.hpp" 

namespace Key {

StackKeys stack_keys = {
    {kKeyRdn, std::make_tuple(
        [](Rpn::Backend& b) -> void { b.Rdn(); },
        "RDN",
        Point{2, 3})},
    {kKeyLastX, std::make_tuple(
        [](Rpn::Backend& b) -> void { b.LastX(); },
        "LASTX",
        Point{3, 3})},
    {kKeySwap, std::make_tuple(
        [](Rpn::Backend& b) -> void { b.SwapXY(); },
        "x<->y",
        Point{1, 3})},
    {kKeyEnter, std::make_tuple(
        [](Rpn::Backend& b) -> void { b.Enter(); },
        "ENTER",
        Point{0, 4})},
    {kKeyPi, std::make_tuple(
        [](Rpn::Backend& b) -> void { b.Pi(); },
        "pi",
        Point{4, 4})},
    {kKeyClx, std::make_tuple(
        [](Rpn::Backend& b) -> void { b.Clx(); },
        "CLX",
        Point{3, 4})},
    {kKeyCls, std::make_tuple(
        [](Rpn::Backend& b) -> void { b.Cls(); },
        "CLS",
        Point{4, 0})},
};

//----------------------------------------------------------------
// Single-argument numeric functions
//----------------------------------------------------------------
SingleArgKeys single_arg_keys = {
    {kKeyChs, std::make_tuple(
        [](double x) -> double { return -x; },
        "chs",
        Point{1, 4})},
    {kKeyInv, std::make_tuple(
        [](double x) -> double { return 1/x; },
        "1/x",
        Point{0, 3})},
    {kKeySin, std::make_tuple(
        [](double x) -> double { return sin(x); },
        "sin",
        Point{1, 1})},
    {kKeyCos, std::make_tuple(
        [](double x) -> double { return cos(x); },
        "cos",
        Point{2, 1})},
    {kKeyTan, std::make_tuple(
        [](double x) -> double { return tan(x); },
        "tan",
        Point{3, 1})},
    {kKeyAsin, std::make_tuple(
        [](double x) -> double { return asin(x); },
        "asin",
        Point{1, 2})},
    {kKeyAcos, std::make_tuple(
        [](double x) -> double { return acos(x); },
        "acos",
        Point{2, 2})},
    {kKeyAtan, std::make_tuple(
        [](double x) -> double { return atan(x); },
        "atan",
        Point{3, 2})},
    {kKeyExp, std::make_tuple(
        [](double x) -> double { return exp(x); },
        "e^x",
        Point{3, 0})},
    {kKeyLn, std::make_tuple(
        [](double x) -> double { return log(x); },
        "ln",
        Point{2, 0})},
    {kKeyLog10, std::make_tuple(
        [](double x) -> double { return log10(x); },
        "log10",
        Point{1, 0})},
    {kKeySqrt, std::make_tuple(
        [](double x) -> double { return sqrt(x); },
        "sqrt",
        Point{0, 1})},
};

//----------------------------------------------------------------
// Double argument numeric functions
//----------------------------------------------------------------
DoubleArgKeys double_arg_keys = {
    {kKeyPlus, std::make_tuple(
        [](double x, double y) -> double { return x + y; },
        "+",
        Point{0, 5})},
    {kKeyMinus, std::make_tuple(
        [](double x, double y) -> double { return y - x; },
        "y-x",
        Point{1, 5})},
    {kKeyMul, std::make_tuple(
        [](double x, double y) -> double { return x * y; },
        "*",
        Point{2, 5})},
    {kKeyDiv, std::make_tuple(
        [](double x, double y) -> double {
            if (std::fabs(x) < 1e-10)
                throw std::invalid_argument("[FATAL]: Backend: Division by zero.\n");
            return y/x; },
        "y/x",
        Point{3, 5})},
    {kKeyPower, std::make_tuple(
        [](double x, double y) -> double { return pow(x, y); },
        "x^y",
        Point{4, 5})}
};

const Keypad keypad{stack_keys,
                   single_arg_keys,
                   double_arg_keys};

} // namespace Key
