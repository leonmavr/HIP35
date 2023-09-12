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
    Screen () = delete;
	// TODO
    Screen (std::vector<std::string> keys);
    ~Screen () {};
	// TODO
	void Draw();

private:
	std::unordered_map<std::string, std::pair<std::string, Point>> key_mappings_;
	std::string active_key_;
	void InitKeypadGrid();
    unsigned key_width;
    unsigned key_height;
};

}

#endif /* SCREEN_HPP */
