#include "screen.hpp"
#include <utility> // make_pair

Gui::Screen::Screen(std::vector<std::string> keys):
	active_key_(""),
    key_width(8),
    key_height(1)
{
	InitKeypadGrid();
}

/**
 * @brief Arranges the keypad given the key mappings (`key_mappings_`).
 *        In the end, we want to arrange the keypad as a 2D grid that
 *        looks as follows:
 *
 *        The screen requires 2 rows as we want to draw a RPN calculator.
 *        The screen displays:
 *        register Y
 *        register X
 *        
 *        Screen
 *        |
 *        | 
 *        v
 *        +----------------------------------- 
 *        | 4.00            
 *        | 20.00
 *        +----------------------------------- <- Keypad
 *        | +       | -       | *        | ...
 *        +----------------------------------- 
 *        | sin (s) | cos (c) | tan (t)  | ...
 *        +----------------------------------- 
 *        ...
 */
void Gui::Screen::InitKeypadGrid() {
	// arithmetic operations
	key_mappings_["+"] =     std::make_pair("+", Gui::Point{0, 2});
	key_mappings_["-"] =     std::make_pair("-", Gui::Point{1, 2});
	key_mappings_["*"] =     std::make_pair("*", Gui::Point{2, 2});
	key_mappings_["/"] =     std::make_pair("/", Gui::Point{3, 2});
	key_mappings_["^"] =     std::make_pair("^", Gui::Point{4, 2});

	key_mappings_["sin"] =   std::make_pair("s", Gui::Point{0, 2});
	key_mappings_["cos"] =   std::make_pair("c", Gui::Point{1, 2});
	key_mappings_["tan"] =   std::make_pair("t", Gui::Point{2, 2});
	key_mappings_["sqrt"] =  std::make_pair("S", Gui::Point{3, 2});
	key_mappings_["log"] =   std::make_pair("l", Gui::Point{4, 2});

	key_mappings_["chs"] =   std::make_pair("C", Gui::Point{0, 2});
	key_mappings_["inv"] =   std::make_pair("i", Gui::Point{1, 2});
    
	// HP35 stack operations
	key_mappings_["RND"] =   std::make_pair("v", Gui::Point{0, 2});
	key_mappings_["SWAP"] =  std::make_pair("(", Gui::Point{1, 2});
	key_mappings_["LASTX"] = std::make_pair("x", Gui::Point{2, 2});
}
