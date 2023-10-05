#ifndef KEYPAD_HPP
#define KEYPAD_HPP 

#include <tuple> // tuple
#include <utility> // make_tuple
#include <memory> // unique_ptr
#include <unordered_map> // unordered_map
#include <string> // string
#include <functional> // function
#include <stdexcept> // invalid_argument
#include <cmath> // invalid_argument

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

namespace Rpn {
    class Backend;
}

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

typedef struct {
    StackKeys stack_keys;
    SingleArgKeys single_arg_keys;
    DoubleArgKeys double_arg_keys;
} Keypad;

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
static std::string AnnotateKey(T& keypad, const std::string keypress) {
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
extern StackKeys stack_keys;
extern SingleArgKeys single_arg_keys;
extern DoubleArgKeys double_arg_keys;

extern const Keypad keypad;
} // namespace Key

#endif /* KEYPAD_HPP */
