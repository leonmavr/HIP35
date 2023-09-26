#ifndef SCREEN_HPP
#define SCREEN_HPP 

#include <unordered_map> // unordered_map
#include <string> // string
#include <vector> // vector
#include <utility> // pair
#include <ncurses.h> // WINDOW 
#include <termios.h> // termios 
#include <stdexcept> // invalid_argument


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
    bool DrawKey(const std::string& key, bool highlight = false);
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
    unsigned key_width_;
    unsigned key_height_;
    unsigned screen_height_;
    unsigned max_width_pixels_;
    unsigned max_height_pixels_;
    bool dimensions_set_;
    // ncurses window (on the terminal) where to draw the keypad
    WINDOW* win_;
    // terminal property settings
    struct termios old_tio_;
    struct termios new_tio_;
};

} // namespace Gui


#endif /* SCREEN_HPP */
