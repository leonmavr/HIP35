#ifndef SCREEN_HPP
#define SCREEN_HPP 

#include <unordered_map> // unordered_map
#include <string> // string
#include <vector> // vector
#include <utility> // pair

namespace Gui {

/**
* @brief Point in the keypad grid with top left as origin (0, 0) 
*/
typedef struct {
    unsigned x, y;
} Point;

class Screen
{
public:
    // TODO: don't delete
    Screen () = delete;
	// TODO: don't need keys to initialize
    Screen (std::vector<std::string> keys);
    ~Screen () { EndTerminal(); }
	// TODO
	void Draw();
    // TODO: private
    void DrawBox(const std::string& function, const std::string& key,
                 const Point& coords);

private:
	void InitKeypadGrid();
    void InitTerminal();
    void EndTerminal();
    // rpn function -> (keypad test, keypad location x,y) e.g. (log -> (l, (4, 2))
	std::unordered_map<std::string, std::pair<std::string, Point>> key_mappings_;
	std::string active_key_;
    unsigned key_width;
    unsigned key_height;
    // ncurses window name (on the terminal) where to draw the keypad
    std::string win_name_; 
};

}

#endif /* SCREEN_HPP */
