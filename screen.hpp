#ifndef SCREEN_HPP
#define SCREEN_HPP 

#include "keypad.hpp"
#include <unordered_map> // unordered_map
#include <string> // string
#include <vector> // vector
#include <utility> // pair
#include <ncurses.h> // WINDOW 
#include <termios.h> // termios 
#include <stdexcept> // invalid_argument
#include <chrono> // milliseconds

namespace Gui {

/**
* @brief Point in the keypad grid with top left as origin (0, 0) 
*/
typedef struct {
    unsigned x, y;
} Point;

class Frontend
{
public:
    Frontend(const Key::Keypad& keypad);
    ~Frontend() { EndTerminal(); }
    /**
     * @brief Draw a calculator's key on the screen. When given a
     *        (valid) key, the frontend knows where to draw it.
     *
     * @param key       Single-length string
     * @param highlight Whether to highlight it (bold, outline, etc.)
     *
     * @return True if the key was valid
     */
    bool DrawKey(const std::string& key, bool highlight = false);
    /**
     * @brief Draw a key, then highlight it for the given number of
     *        milliseconds, then un-highlight it.
     *
     * @param key Same as in DrawKey
     * @param ms  Duration in ms to keep the key highlighted
     *
     * @return Same as DrawKey
     */
    bool HighlightKey(const std::string& key,
                      std::chrono::milliseconds ms = std::chrono::milliseconds(100));
    bool PrintRegisters(double regx, double regy);
    // this operator is used to return data (long function names)
    // from key_mappings_
    std::string operator[](const std::string& key) const {
        auto it = key_mappings_.find(key);
        if (it == key_mappings_.end())
            return "";
        return key_mappings_.at(key).first;
    }
    void PrintGenRegister(const std::string& name, double val);

private:
	void InitKeypadGrid();
    void InitTerminal();
    /**
     * @brief Restores the terminal to the state before it was set.
     *        Deletes various ncurses structures. Meant to be called 
     *        upon deletion of the instance.
     */
    void EndTerminal();
    bool DrawKeypad();
    bool DrawDisplay();
    void DrawBox(const std::string& text, const Point& coords,
                 bool highlight = false);
    /**
     * @brief Set the dimsnsions of the UI given the keypad grid
     *        and the key dimensions
     */
    void SetUiDimensions();
    // keypad short text -> (keypad long text, keypad location x,y) e.g.
    // (l -> (long, (4, 2))
	std::unordered_map<std::string,
                       std::pair<std::string, Point>> key_mappings_;
	std::string active_key_;
    // width, height in characters
    unsigned key_width_;
    unsigned key_height_;
    unsigned screen_height_;
    // also in characters
    unsigned max_width_pixels_;
    unsigned max_height_pixels_;
    // whether max_width_pixels_, max_height_pixels_ were set
    bool dimensions_set_;
    // ncurses window (on the terminal) where to draw the keypad
    WINDOW* win_;
    // terminal property settings
    struct termios old_tio_;
    struct termios new_tio_;
    // reference to keypad - we draw the interface based on it
    const Key::Keypad& keypad_;
    // screen width in characters, not including the frame
    unsigned screen_width_;
    //------------------------------------------------------
    // 10 general registers
    //------------------------------------------------------
    // top left position of the general register display area
    Key::Point gen_regs_top_left_;
    std::unordered_map<std::string, Key::Point> gen_reg_area_;
    // how many characters each general register can display
    unsigned gen_reg_width_;
    // maps the display key of each gen. register to its coordinates
    std::unordered_map<std::string, Key::Point> gen_regs_;
};

/**
 * @brief  Formats a number to a string that represents it in
 *         scientific format. The scientific format is defined as
 *         follows:  
 *         m * 10^E  
 *         m is the mantissa, with 1 <= |m| < 10. It can have a given
 *         precision of decimals. E is the exponent. Any trailing
 *         zeros are truncated from the mantissa, so 1.20 would
 *         become 1.2. Examples (`E-4` denotes `10^(-4)`):  
 *         @verbatim
 *          -.000123456, 3  -> -1.235 E-4 
 *          -.000123456, 14 -> -1.2356 E-4 
 *          123.12345,   2  ->  1.23, E2
 *          @endverbatim
 *
 * @param number    Number to convert
 * @param precision The number of decimals of the mantissa
 *
 * @return A string representing the number in scientific format:  
 *         m * 10^E
 */
static std::string FmtEngineeringNotation(double number, int precision = 5);
/**
 * @brief  Formats a number in fixed decimal precision format, e.g.
 *         with precision = 2:  
 *          3.14159 ->  3.14
 *         -3.1     -> -3.10
 *         NOTE: trailing zeros are preserved.
 *
 * @param num  A number in 64-bit signed double range
 * @param prec How many decimals to retain in the format 
 *
 * @return A number string formatted in fixed precision
 */
static std::string FmtFixedPrecision(double num, unsigned prec = 5);

/**
 * @brief Formats a numbers base on its magnitude. It selects format
 *        (fixed precision/engineering) and number of decimals. The 
 *        scheme is somewhat arbitrary but works well for a 
 *        calculator.
 *
 * @param num          A number in 64-bit signed double range
 * @param screen_width Calculator's screen width. Required to pad 
 *                     the number with spaces and avoid redrawing 
 *                     with ncurses.
 *
 * @return A formatted number string 
 */
static inline std::string FmtBasedOnRange(double num, unsigned screen_width);



/**
 * @brief Pads the beginning of a string with spaces up to a specified 
 *        length.
 *
 * @param input Input string (read only)
 * @param N     The desired length
 *
 * @return The padded version of the input string
 */
static std::string padString(const std::string& input, std::size_t N);

} // namespace Gui
#endif /* SCREEN_HPP */
