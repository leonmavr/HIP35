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
const std::string kKeyRdn   = "v", kKeyRdnLong   = "RND";
const std::string kKeyLastX = "x", kKeyLastXLong = "LASTX";
const std::string kKeySwap  = "<", kKeySwapLong  = "SWAP";
const std::string kKeyEnter = " ", kKeyEnterLong = "ENTER";
const std::string kKeyPi    = "p", kKeyPiLong    = "PI";
const std::string kKeyClx   = "@", kKeyClxLong   = "CLX";
const std::string kKeyCls   = "$", kKeyClsLong   = "CLS";
// numerical operations with 1 argument
const std::string kKeyChs   = "!", kKeyChsLong   = "CHS";
const std::string kKeyInv   = "i", kKeyInvLong   = "INV";
const std::string kKeySin   = "s", kKeySinLong   = "SIN";
const std::string kKeyCos   = "c", kKeyCosLong   = "COS";
const std::string kKeyTan   = "t", kKeyTanLong   = "TAN";
const std::string kKeyAsin  = "S", kKeyAsinLong  = "ASIN";
const std::string kKeyAcos  = "C", kKeyAcosLong  = "ACOS";
const std::string kKeyAtan  = "T", kKeyAtanLong  = "ATAN";
const std::string kKeyExp   = "e", kKeyExpLong   = "EXP";
const std::string kKeyLn    = "l", kKeyLnLong    = "LN";
const std::string kKeyLog10 = "L", kKeyLog10Long = "LOG10";
const std::string kKeySqrt  = "r", kKeySqrtLong  = "SQRT";
// numerical operations with 2 arguments
const std::string kKeyPlus  = "+", kKeyPlusLong  = "+";
const std::string kKeyMinus = "-", kKeyMinusLong = "-";
const std::string kKeyMul   = "*", kKeyMulLong   = "*";
const std::string kKeyDiv   = "/", kKeyDivLong   = "/";
const std::string kKeyPower = "^", kKeyPowerLong = "^";
// prefix operations (1 argument)
const std::string kKeyRcl   = "?", kKeyRclLong   = "RCL";
const std::string kKeyStore = "#", kKeyStoreLong = "STO";
const std::string kKeyEex   = "E", kKeyEexLong   = "EEX";

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


/**
 * @brief Maps the following:
 *        @verbatim
 *        keypress --+--> function: 
 *                   |    Can be a function of one or two registers  
 *        Single     |    or a backend instance or a backend instance
 *        length     |    and an index.
 *        key        |
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
using StackKeys = std::unordered_map<std::string, std::tuple<
                                std::function<void(Rpn::Backend& b)>,
                                std::string,
                                Point,
                                std::string>>;

/** @copydoc StackKeys */
using SingleArgKeys = std::unordered_map<std::string, std::tuple<
                                std::function<double(double)>,
                                std::string,
                                Point,
                                std::string>>;

/** @copydoc StackKeys */
using DoubleArgKeys = std::unordered_map<std::string, std::tuple<
                                std::function<double(double, double)>,
                                std::string,
                                Point,
                                std::string>>;

/** @copydoc StackKeys */
using StorageKeys = std::unordered_map<std::string, std::tuple<
                                std::function<void(Rpn::Backend& b, std::string name)>,
                                std::string,
                                Point,
                                std::string>>;

/** @copydoc StackKeys */
using EexKey = std::unordered_map<std::string, std::tuple<
                                std::function<void(Rpn::Backend& b, std::optional<double> operand)>,
                                std::string,
                                Point,
                                std::string>>;

typedef struct {
    StackKeys stack_keys;
    SingleArgKeys single_arg_keys;
    DoubleArgKeys double_arg_keys;
    StorageKeys storage_keys;
    EexKey eex_key;
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
    const auto& tuple = it->second;
    const std::string& key_long = std::get<1>(tuple);
    if (keypress == key_long)
        ret = keypress;
    else
        ret = key_long + " (" + keypress + ")";
    return ret;
}

//----------------------------------------------------------------
// keypad mapping types to be used by frontend and backend 
//----------------------------------------------------------------
extern const StackKeys stack_keys;
extern const SingleArgKeys single_arg_keys;
extern const DoubleArgKeys double_arg_keys;
extern const StorageKeys storage_keys;
extern const EexKey eex_key;
extern const Keypad keypad;

} // namespace Key

#endif /* KEYPAD_HPP */
