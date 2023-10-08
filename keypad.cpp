#include "keypad.hpp" 
#include "backend.hpp" 

namespace Key {

StackKeys stack_keys = {
    {kKeyRdn, std::make_tuple(
        [](Rpn::Backend& b) -> void { b.Rdn(); },
        "RDN",
        Point{2, 3},
        "RDN")},
    {kKeyLastX, std::make_tuple(
        [](Rpn::Backend& b) -> void { b.LastX(); },
        "LASTX",
        Point{3, 3},
        "LASTX")},
    {kKeySwap, std::make_tuple(
        [](Rpn::Backend& b) -> void { b.SwapXY(); },
        "x<->y",
        Point{1, 3},
        "SWAP")},
    {kKeyEnter, std::make_tuple(
        [](Rpn::Backend& b) -> void { b.Enter(); },
        "ENTER",
        Point{0, 4},
        "ENTER")},
    {kKeyPi, std::make_tuple(
        [](Rpn::Backend& b) -> void { b.Pi(); },
        "pi",
        Point{4, 4},
        "PI")},
    {kKeyClx, std::make_tuple(
        [](Rpn::Backend& b) -> void { b.Clx(); },
        "CLX",
        Point{3, 4},
        "CLX")},
    {kKeyCls, std::make_tuple(
        [](Rpn::Backend& b) -> void { b.Cls(); },
        "CLS",
        Point{4, 0},
        "CLS")},
};

//----------------------------------------------------------------
// Single-argument numeric functions
//----------------------------------------------------------------
SingleArgKeys single_arg_keys = {
    {kKeyChs, std::make_tuple(
        [](double x) -> double { return -x; },
        "chs",
        Point{1, 4},
        "CHS")},
    {kKeyInv, std::make_tuple(
        [](double x) -> double { return 1/x; },
        "1/x",
        Point{0, 3},
        "INV")},
    {kKeySin, std::make_tuple(
        [](double x) -> double { return sin(x); },
        "sin",
        Point{1, 1},
        "SIN")},
    {kKeyCos, std::make_tuple(
        [](double x) -> double { return cos(x); },
        "cos",
        Point{2, 1},
        "COS")},
    {kKeyTan, std::make_tuple(
        [](double x) -> double { return tan(x); },
        "tan",
        Point{3, 1},
        "TAN")},
    {kKeyAsin, std::make_tuple(
        [](double x) -> double { return asin(x); },
        "asin",
        Point{1, 2},
        "ASIN")},
    {kKeyAcos, std::make_tuple(
        [](double x) -> double { return acos(x); },
        "acos",
        Point{2, 2},
        "ACOS")},
    {kKeyAtan, std::make_tuple(
        [](double x) -> double { return atan(x); },
        "atan",
        Point{3, 2},
        "ATAN")},
    {kKeyExp, std::make_tuple(
        [](double x) -> double { return exp(x); },
        "e^x",
        Point{3, 0},
        "EXP")},
    {kKeyLn, std::make_tuple(
        [](double x) -> double { return log(x); },
        "ln",
        Point{2, 0},
        "LN")},
    {kKeyLog10, std::make_tuple(
        [](double x) -> double { return log10(x); },
        "log10",
        Point{1, 0},
        "LOG10")},
    {kKeySqrt, std::make_tuple(
        [](double x) -> double { return sqrt(x); },
        "sqrt",
        Point{0, 1},
        "SQRT")},
};

//----------------------------------------------------------------
// Double argument numeric functions
//----------------------------------------------------------------
DoubleArgKeys double_arg_keys = {
    {kKeyPlus, std::make_tuple(
        [](double x, double y) -> double { return x + y; },
        "+",
        Point{0, 5},
        "+")},
    {kKeyMinus, std::make_tuple(
        [](double x, double y) -> double { return y - x; },
        "y-x",
        Point{1, 5},
        "-")},
    {kKeyMul, std::make_tuple(
        [](double x, double y) -> double { return x * y; },
        "*",
        Point{2, 5},
        "*")},
    {kKeyDiv, std::make_tuple(
        [](double x, double y) -> double {
            if (std::fabs(x) < 1e-10)
                throw std::invalid_argument("[FATAL]: Backend: Division by zero.\n");
            return y/x; },
        "y/x",
        Point{3, 5},
        "/")},
    {kKeyPower, std::make_tuple(
        [](double x, double y) -> double { return pow(x, y); },
        "x^y",
        Point{4, 5},
        "^")}
};

const Keypad keypad{stack_keys,
                   single_arg_keys,
                   double_arg_keys};

} // namespace Key