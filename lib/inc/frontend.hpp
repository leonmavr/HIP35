#ifndef SCREEN_HPP
#define SCREEN_HPP 

#include "keypad.hpp"
#include <unordered_map> // unordered_map
#include <string>        // string
#include <vector>        // vector
#include <utility>       // pair
#include <ncurses.h>     // WINDOW 
#include <termios.h>     // termios 
#include <chrono>        // milliseconds

namespace gui {
/**
* @brief Point in the keypad grid with top left as origin (0, 0) 
*/
typedef struct {
    unsigned x, y;
} Point;

//-------------------------------------------------------------//
// Class methods                                               //
//-------------------------------------------------------------//
class Frontend
{
public:
    Frontend(const key::Keypad& keypad);
    ~Frontend();
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
    void PrintGenRegister(const std::string& name, double val);
    /**
     * @brief Restores the terminal to the state before it was set.
     *        Deletes various ncurses structures.
     */
    void CloseUi();
    // this operator is used to return data (long function names)
    // from key_mappings_
    std::string operator[](const std::string& key) const {
        auto it = key_mappings_.find(key);
        if (it == key_mappings_.end())
            return "";
        return key_mappings_.at(key).first;
    }

private:
    /**
    * @brief Arranges the keypad given the keypad config (`keypad_`).
    *        In the end, we want to arrange the keypad as a 2D grid that
    *        looks as follows:
    *
    *        @verbatim
    *        screen
    *        |
    *        | 
    *        v
    *        +----------------------------------- 
    *        |                              4.00  <- register Y
    *        |                             20.00  <- register X
    *        +---------+---------+----------+---- <- keypad
    *        | +       | -       | *        | ...
    *        +---------+---------+----------+----
    *        | sin (s) | cos (c) | tan (t)  | ...
    *        +---------+---------+----------+---- 
    *        ...
    *        @endverbatim
    */
	void InitKeypadGrid();
    void InitTerminal();
    bool DrawKeypad();
    bool DrawDisplay();
    void DrawBox(const std::string& text, const Point& coords,
                 bool highlight = false);
    /**
     * @brief Set the dimsnsions of the UI given the keypad grid
     *        and the key dimensions
     */
    void SetUiDimensions();

    // reference to keypad - we draw the interface based on it
    const key::Keypad& keypad_;
    //------------------------------------------------------
    // screen and keypad dimensions 
    //------------------------------------------------------
    // width, height in characters
    unsigned key_width_;
    unsigned key_height_;
    unsigned screen_height_;
    // screen width in characters, not including the frame
    unsigned screen_width_;
    // also in characters
    unsigned max_width_pixels_;
    unsigned max_height_pixels_;
    // whether max_width_pixels_, max_height_pixels_ were set
    bool dimensions_set_;
    // keypad short text -> (keypad long text, keypad location x,y) e.g.
    // (l -> (long, (4, 2))
	std::unordered_map<std::string,
                       std::pair<std::string, Point>> key_mappings_;
    //------------------------------------------------------
    // 10 general registers
    //------------------------------------------------------
    // top left position of the general register display area
    key::Point gen_regs_top_left_;
    std::unordered_map<std::string, key::Point> gen_reg_area_;
    // how many characters each general register can display
    unsigned gen_reg_width_;
    // maps the display key of each gen. register to its coordinates
    std::unordered_map<std::string, key::Point> gen_regs_;
    //------------------------------------------------------
    // ncurses and terminal 
    //------------------------------------------------------
    // ncurses window (on the terminal) where to draw the keypad
    WINDOW* win_;
    // terminal property settings
    struct termios old_tio_;
    struct termios new_tio_;
};

} // namespace gui
#endif /* SCREEN_HPP */
