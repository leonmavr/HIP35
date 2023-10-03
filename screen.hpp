#ifndef SCREEN_HPP
#define SCREEN_HPP 

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
    Frontend ();
    ~Frontend () { EndTerminal(); }
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
     * @brief 
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

private:
	void InitKeypadGrid();
    void InitTerminal();
    void EndTerminal();
    bool DrawKeypad();
    bool DrawScreen();
    void DrawBox(const std::string& text, const Point& coords,
                 bool highlight = false);
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
};

} // namespace Gui


#endif /* SCREEN_HPP */
