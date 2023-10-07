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

// define key presses to call calculator functions
// for their implementation see the mapping in this file
// keys that manipulate the stack
const std::string kKeyRdn   = "v";
const std::string kKeyLastX = "x";
const std::string kKeySwap  = "<";
const std::string kKeyEnter = " ";
const std::string kKeyPi    = "p";
const std::string kKeyClx   = "@";
const std::string kKeyCls   = "$";
// numerical operations with 1 argument
const std::string kKeyChs   = "!";
const std::string kKeyInv   = "i";
const std::string kKeySin   = "s";
const std::string kKeyCos   = "c";
const std::string kKeyTan   = "t";
const std::string kKeyAsin  = "S";
const std::string kKeyAcos  = "C";
const std::string kKeyAtan  = "T";
const std::string kKeyExp   = "e";
const std::string kKeyLn    = "l";
const std::string kKeyLog10 = "L";
const std::string kKeySqrt  = "r";
// numerical operations with 2 arguments
const std::string kKeyPlus  = "+";
const std::string kKeyMinus = "-";
const std::string kKeyMul   = "*";
const std::string kKeyDiv   = "/";
const std::string kKeyPower = "^";

/**
* @brief Point in the keypad grid with top left as origin (0, 0) 
*/
typedef struct {
    unsigned x, y;
} Point;


// alias the really long key types
/**
 * @brief 
 */
using StackKeys = std::unordered_map<std::string, std::tuple<
                                std::function<void(Rpn::Backend& b)>,
                                std::string,
                                Point,
                                std::string>>;

using SingleArgKeys = std::unordered_map<std::string, std::tuple<
                                std::function<double(double)>,
                                std::string,
                                Point,
                                std::string>>;

using DoubleArgKeys = std::unordered_map<std::string, std::tuple<
                                std::function<double(double, double)>,
                                std::string,
                                Point,
                                std::string>>;

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
static std::string AnnotateKey(T& it, const std::string keypress) {
    std::string ret = "";
    const std::string& key_short = keypress;
    const auto& tuple = it->second;
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
