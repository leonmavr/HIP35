#ifndef KEYPAD_HPP
#define KEYPAD_HPP 

#include "backend.hpp" 
#include <tuple> // tuple
#include <utility> // make_tuple
#include <memory> // unique_ptr
#include <unordered_map> // unordered_map
#include <string> // string
#include <functional> // function
#include <stdexcept> // invalid_argument

// define key presses to call calculator functions
// for their implementation see the mapping in this file
// keys that manipulate the stack
#define KEY_RDN   "v"
#define KEY_LASTX "x"
#define KEY_SWAP  "<"
#define KEY_ENTER " "
#define KEY_PI    "p"
#define KEY_CLX   "@"
#define KEY_CLS   "$"
// numerical operations with 1 argument
#define KEY_CHS   "!"
#define KEY_INV   "i"
#define KEY_SIN   "s"
#define KEY_COS   "c"
#define KEY_TAN   "t"
#define KEY_ASIN  "S"
#define KEY_ACOS  "C"
#define KEY_ATAN  "T"
#define KEY_EXP   "e"
#define KEY_LN    "l"
#define KEY_LOG10 "L"
#define KEY_SQRT  "r"
// numerical operations with 2 arguments
#define KEY_PLUS  "+"
#define KEY_MINUS "-"
#define KEY_MUL   "*"
#define KEY_DIV   "/"
#define KEY_POWER "^"

/**
 * @brief Namespace that encompasses the calculator's keypad.
 *        Keypad is used by the backend to map a function
 *        key to its implementation (e.g. + -> x+y, l -> ln(x))
 *        and by the frontend to interact with the user via
 *        key presses and to draw the keypad, draw text on the
 *        keypad (e.g. + -> x+y, + -> +, l -> ln(x), l ->
 *        "ln (l)"). It uses the following universal mapping:
 *
 *        key to press -> (function, descriptor, grid (x, y))
 *
 *        Grid (x, y) refers to the relative position of the
 *        keys w/o taking width and height into account. So
 *        two adjacent keys will be (x, y), (x+1, y).
 */
namespace Key {
/**
* @brief Point in the keypad grid with top left as origin (0, 0) 
*/
typedef struct {
    unsigned x, y;
} Point;


// alias the really long key types
using StackKeys = std::unordered_map<std::string, std::tuple<
                                std::function<void(Rpn::Backend& b)>,
                                std::string,
                                Point>>;

using SingleArgKeys = std::unordered_map<std::string, std::tuple<
                                std::function<double(double)>,
                                std::string,
                                Point>>;

using DoubleArgKeys = std::unordered_map<std::string, std::tuple<
                                std::function<double(double, double)>,
                                std::string,
                                Point>>;

/**
 * @brief Compiles a description of the input key as long as
 *        it's found in the keypad; e.g. if the input keypress
 *        is "s" and the long key description is SIN, it could
 *        return SIN (s) or however we decide to annotate them.
 *
 * @param keypad   A keypad description object. Unordered map
 *                 which describes a number of keys.
 * @param keypress Single-length string.
 *
 * @return         The annotation of `key` given its entry in
 *                 `keypad`. Empty string if not found.
 */
template <typename T>
std::string AnnotateKey(T& keypad, const std::string keypress) {
    std::string ret = "";
    auto it = keypad.find(keypress);
    if (it == keypad.end())
        return ret;
    const std::string& key_short = keypress;
    const auto& tuple = keypad[keypress];
    const std::string& key_long = std::get<1>(tuple);
    if (key_short == key_long)
        ret = key_short;
    else
        ret = key_long + " (" + key_short + ")";
    return ret;
}

//----------------------------------------------------------------
// stack-altering functions
//----------------------------------------------------------------
StackKeys function_key_0op_ = {
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
SingleArgKeys function_key_1op_ = {
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
DoubleArgKeys function_key_2op_ = {
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

} // namespace Key

#endif /* KEYPAD_HPP */
