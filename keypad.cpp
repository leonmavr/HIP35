#include "keypad.hpp" 
#include "backend.hpp" 

namespace Key {

StackKeys stack_keys = {
    {KEY_RDN, std::make_tuple(
        [](Rpn::Backend& b) -> void { b.Rdn(); },
        "RDN",
        Point{2, 3})},
    {KEY_LASTX, std::make_tuple(
        [](Rpn::Backend& b) -> void { b.LastX(); },
        "LASTX",
        Point{3, 3})},
    {KEY_SWAP, std::make_tuple(
        [](Rpn::Backend& b) -> void { b.SwapXY(); },
        "x<->y",
        Point{1, 3})},
    {KEY_ENTER, std::make_tuple(
        [](Rpn::Backend& b) -> void { b.Enter(); },
        "ENTER",
        Point{0, 4})},
    {KEY_PI, std::make_tuple(
        [](Rpn::Backend& b) -> void { b.Pi(); },
        "pi",
        Point{4, 4})},
    {KEY_CLX, std::make_tuple(
        [](Rpn::Backend& b) -> void { b.Clx(); },
        "CLX",
        Point{3, 4})},
    {KEY_CLS, std::make_tuple(
        [](Rpn::Backend& b) -> void { b.Cls(); },
        "CLS",
        Point{4, 0})},
};

//----------------------------------------------------------------
// Single-argument numeric functions
//----------------------------------------------------------------
SingleArgKeys single_arg_keys = {
    {KEY_CHS, std::make_tuple(
        [](double x) -> double { return -x; },
        "chs",
        Point{1, 4})},
    {KEY_INV, std::make_tuple(
        [](double x) -> double { return 1/x; },
        "1/x",
        Point{0, 3})},
    {KEY_SIN, std::make_tuple(
        [](double x) -> double { return sin(x); },
        "sin",
        Point{1, 1})},
    {KEY_COS, std::make_tuple(
        [](double x) -> double { return cos(x); },
        "cos",
        Point{2, 1})},
    {KEY_TAN, std::make_tuple(
        [](double x) -> double { return tan(x); },
        "tan",
        Point{3, 1})},
    {KEY_ASIN, std::make_tuple(
        [](double x) -> double { return asin(x); },
        "asin",
        Point{1, 2})},
    {KEY_ACOS, std::make_tuple(
        [](double x) -> double { return acos(x); },
        "acos",
        Point{2, 2})},
    {KEY_ATAN, std::make_tuple(
        [](double x) -> double { return atan(x); },
        "atan",
        Point{3, 2})},
    {KEY_EXP, std::make_tuple(
        [](double x) -> double { return exp(x); },
        "e^x",
        Point{3, 0})},
    {KEY_LN, std::make_tuple(
        [](double x) -> double { return log(x); },
        "ln",
        Point{2, 0})},
    {KEY_LOG10, std::make_tuple(
        [](double x) -> double { return log10(x); },
        "log10",
        Point{1, 0})},
    {KEY_SQRT, std::make_tuple(
        [](double x) -> double { return sqrt(x); },
        "sqrt",
        Point{0, 1})},
};

//----------------------------------------------------------------
// Double argument numeric functions
//----------------------------------------------------------------
DoubleArgKeys double_arg_keys = {
    {KEY_PLUS, std::make_tuple(
        [](double x, double y) -> double { return x + y; },
        "+",
        Point{0, 5})},
    {KEY_MINUS, std::make_tuple(
        [](double x, double y) -> double { return y - x; },
        "y-x",
        Point{1, 5})},
    {KEY_MUL, std::make_tuple(
        [](double x, double y) -> double { return x * y; },
        "*",
        Point{2, 5})},
    {KEY_DIV, std::make_tuple(
        [](double x, double y) -> double {
            if (std::fabs(x) < 1e-10)
                throw std::invalid_argument("[FATAL]: Backend: Division by zero.\n");
            return y/x; },
        "y/x",
        Point{3, 5})},
    {KEY_POWER, std::make_tuple(
        [](double x, double y) -> double { return pow(x, y); },
        "x^y",
        Point{4, 5})}
};

const Keypad keypad{stack_keys,
                   single_arg_keys,
                   double_arg_keys};

} // namespace Key
