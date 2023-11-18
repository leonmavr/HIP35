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


//// Key definitions
// Short = keypresses the uses will type when running the UI
//         e.g. 10 L 1 +
// Long  = long code for the key when evaluation an expression
//         without using the UI, e.g. "10 LOG10 1 +"

// stack operations
#define KEY_LASTX_SHORT "x"
#define KEY_LASTX_LONG  "LASTX"
#define KEY_SWAP_SHORT  "<"
#define KEY_SWAP_LONG   "SWAP"
#define KEY_ENTER_SHORT " "
#define KEY_ENTER_LONG  "ENTER"
#define KEY_PI_SHORT    "p"
#define KEY_PI_LONG     "PI"
#define KEY_CLX_SHORT   "@"
#define KEY_CLX_LONG    "CLX"
#define KEY_CLS_SHORT   "$"
#define KEY_CLS_LONG    "CLS"
// monadic operations
#define KEY_CHS_SHORT   "!"
#define KEY_CHS_LONG    "CHS"
#define KEY_INV_SHORT   "i"
#define KEY_INV_LONG    "INV"
#define KEY_SIN_SHORT   "s"
#define KEY_SIN_LONG    "SIN"
#define KEY_COS_SHORT   "c"
#define KEY_COS_LONG    "COS"
#define KEY_TAN_SHORT   "t"
#define KEY_TAN_LONG    "TAN"
#define KEY_ASIN_SHORT  "S"
#define KEY_ASIN_LONG   "ASIN"
#define KEY_ACOS_SHORT  "C"
#define KEY_ACOS_LONG   "ACOS"
#define KEY_ATAN_SHORT  "T"
#define KEY_ATAN_LONG   "ATAN"
#define KEY_EXP_SHORT   "e"
#define KEY_EXP_LONG    "EXP"
#define KEY_LN_SHORT    "l"
#define KEY_LN_LONG     "LN"
#define KEY_LOG10_SHORT "L"
#define KEY_LOG10_LONG  "LOG10"
#define KEY_SQRT_SHORT  "r"
#define KEY_SQRT_LONG   "SQRT"
// dual operations
#define KEY_PLUS_SHORT  "+"
#define KEY_PLUS_LONG   "+"
#define KEY_MINUS_SHORT "-"
#define KEY_MINUS_LONG  "-"
#define KEY_MUL_SHORT   "*"
#define KEY_MUL_LONG    "*"
#define KEY_DIV_SHORT   "/"
#define KEY_DIV_LONG    "/"
#define KEY_POW_SHORT   "^"
#define KEY_POW_LONG    "^"
// recall/store
#define KEY_RCL_SHORT   "?"
#define KEY_RCL_LONG    "RCL"
#define KEY_STO_SHORT   "#"
#define KEY_STO_LONG    "STO"
// EEX key
#define KEY_EEX_SHORT   "E"
#define KEY_EEX_LONG    "EEX"

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
const std::string kKeyRdn   = "v", kKeyRdnLong   = "RDN";
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
extern std::unordered_map<std::string, std::string> reverse_keys;


} // namespace Key

#endif /* KEYPAD_HPP */
