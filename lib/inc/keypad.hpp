#ifndef KEYPAD_HPP
#define KEYPAD_HPP 

#include <tuple>         // tuple
#include <utility>       // make_tuple
#include <memory>        // unique_ptr
#include <unordered_map> // unordered_map
#include <string>        // string
#include <functional>    // function
#include <stdexcept>     // invalid_argument
#include <cmath>         // invalid_argument
#include <array>         // array 
#include <optional>      // optional 

// Forward-declaration of class `Backend` to resolve the
// circular dependency keypad -> backend -> keypad
namespace backend {
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
namespace key {

// define key presses to call calculator functions
// for their implementation see the mapping in this file
// keys that manipulate the stack
const std::string kKeyRdn   = "v";
const std::string kKeyLastX = "x";
const std::string kKeySwap  = "<";
const std::string kKeyEnter = " ";
const std::string kKeyPi    = "p";
const std::string kKeyClx   = "@";
const std::string kKeyClr   = "$";
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
// prefix operations (1 argument)
const std::string kKeyRcl   = "?";
const std::string kKeyStore = "#";
const std::string kKeyEex   = "E";

/**
 *  @brief Names for the 10 general registers - MUST be one letter.
 *         Names are not case sensitive so please don't use the same
 *         letter for two general registers, e.g. A and a
 */
const std::array<std::string, 10> kNamesGenRegs = {"A", "B", "C", "D", "E",
                                                   "F", "G", "H", "I", "J"};
/** @brief Point in the keypad grid with top left as origin (0, 0) */
typedef struct {
    unsigned x, y;
} Point;


struct StackKeyInfo {
    std::function<void(backend::Backend& b)> function;
    std::string annotation;
    key::Point point;
    std::string long_key;
};
/**
 * @brief Maps the following:
 *        @verbatim
 *        keypress --+--> function: 
 *           |       |    Can be a function of one or two registers  
 *        (Single    |    or a backend instance or a backend instance
 *        length     |    and an index.
 *        key)       |
 *                   +--> annotation:
 *                   |    The annoration of the keypress on the
 *                   |    frontend, e.g. for `s` it can be `SIN` 
 *                   |
 *                   +--> grid location:
 *                   |    The (x, y) position on the keypad grid.
 *                   |    Each key occupies its own (x, y)
 *                   |
 *                   +--> long key:
 *                        Long description of the key. This helps
 *                        entering the key in non-interactive mode.
 *                        e.g. instead of entering `S`, we enter
 *                        `ARCSIN`. Different form the annotation.
 *        @endverbatim
 */
using StackKeys = std::unordered_map<std::string, StackKeyInfo>; 

struct SingleKeyInfo {
    std::function<double(double)> function;
    std::string annotation;
    key::Point point;
    std::string long_key;
};
/** @copydoc StackKeys */
using SingleArgKeys = std::unordered_map<std::string, SingleKeyInfo>;

/** @copydoc StackKeys */
struct DoubleKeyInfo {
    std::function<double(double, double)> function;
    std::string annotation;
    key::Point point;
    std::string long_key;
};
using DoubleArgKeys = std::unordered_map<std::string, DoubleKeyInfo>;

struct StorageKeyInfo {
    std::function<void(backend::Backend& b, std::string name)> function;
    std::string annotation;
    key::Point point;
    std::string long_key;
};
/** @copydoc StackKeys */
using StorageKeys = std::unordered_map<std::string, StorageKeyInfo>;

struct KeyInfoEex {
    std::function<void(backend::Backend&, std::optional<double>)> function;
    std::string annotation;
    key::Point point;
    std::string long_key;
};
/** @copydoc StackKeys */
using EexKey = std::unordered_map<std::string, KeyInfoEex>;


typedef struct {
    StackKeys stack_keys;
    SingleArgKeys single_arg_keys;
    DoubleArgKeys double_arg_keys;
    StorageKeys storage_keys;
    EexKey eex_key;
    std::unordered_map<std::string, std::string> reverse_keys;
} Keypad;

/**
 * @brief Compiles a description of the input key as long as
 *        it's found in the keypad; e.g. if the input keypress
 *        is `s` and the long key description is SIN, it could
 *        return `SIN (s)` or however we decide to annotate them.
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
    const std::string& annot = it->second.annotation;
    if (keypress == annot)
        ret = keypress;
    else
        ret = annot + " (" + keypress + ")";
    return ret;
}

//----------------------------------------------------------------
// keypad mapping types to be used by frontend and backend 
//----------------------------------------------------------------
extern const Keypad keypad;


} // namespace key

#endif /* KEYPAD_HPP */
