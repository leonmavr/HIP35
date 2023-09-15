#include "screen.hpp"
#include <utility> // make_pair
#include <ncurses.h>

Gui::Screen::Screen():
	active_key_(""),
    key_width(9),
    key_height(3)
{
    InitTerminal();
	InitKeypadGrid();
    // prepare the terminal for drawing 
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
 *        +---------+---------+----------+---- <- Keypad
 *        | +       | -       | *        | ...
 *        +---------+---------+----------+----
 *        | sin (s) | cos (c) | tan (t)  | ...
 *        +---------+---------+----------+---- 
 *        ...
 */
void Gui::Screen::InitKeypadGrid() {
	// arithmetic operations
    //     text on button                text in ()         x, y
	key_mappings_["+"] =     std::make_pair("+", Gui::Point{0, 2});
	key_mappings_["-"] =     std::make_pair("-", Gui::Point{1, 2});
	key_mappings_["*"] =     std::make_pair("*", Gui::Point{2, 2});
	key_mappings_["/"] =     std::make_pair("/", Gui::Point{3, 2});
	key_mappings_["^"] =     std::make_pair("^", Gui::Point{4, 2});

	key_mappings_["sin"] =   std::make_pair("s", Gui::Point{0, 3});
	key_mappings_["cos"] =   std::make_pair("c", Gui::Point{1, 3});
	key_mappings_["tan"] =   std::make_pair("t", Gui::Point{2, 3});
	key_mappings_["sqrt"] =  std::make_pair("S", Gui::Point{3, 3});
	key_mappings_["log"] =   std::make_pair("l", Gui::Point{4, 3});

	key_mappings_["chs"] =   std::make_pair("C", Gui::Point{0, 4});
	key_mappings_["inv"] =   std::make_pair("i", Gui::Point{1, 4});
    
	// HP35 stack operations
	key_mappings_["RND"] =   std::make_pair("v", Gui::Point{0, 5});
	key_mappings_["SWAP"] =  std::make_pair("<", Gui::Point{1, 5});
	key_mappings_["LASTX"] = std::make_pair("x", Gui::Point{2, 5});
}

void Gui::Screen::DrawFrame() {

}

void Gui::Screen::DrawBox(const std::string& function, const std::string& key,
                          const Gui::Point& coords) {
    const unsigned x = coords.x;
    const unsigned y = coords.y;
    const int nlines = Gui::Screen::key_height * 7;
    const int ncols = Gui::Screen::key_width * 8;
    // not sure if i'll need this
    // TODO: move to c/tor
    WINDOW * win = newwin(nlines, ncols, 0, 0);
    const int w = Gui::Screen::key_width;
    const int h = Gui::Screen::key_height;
    //            l    r    t    d   tl   tr   bl   br
    wborder(win, '.', '.', '.', '.', '.', '.', '.', '.');

    // top left corner
    wmove(win, y, x);
    wprintw(win, "+");
    // top edge
    wmove(win, y, x+1);
    whline(win, '-', w-2);
    // top right corner
    wmove(win, y, x+w-1);
    wprintw(win, "+");
    // right edge
    wmove(win, y+1, x+w-1);
    wprintw(win, "|");
    // bottom right corner
    wmove(win, y+2, x+w-1);
    wprintw(win, "+");
    // bottom edge
    wmove(win, y+2, x+1);
    whline(win, '-', w-2);
    // bottom left corner
    wmove(win, y+2, x);
    wprintw(win, "+");
    // left edge
    wmove(win, y+1, x);
    wprintw(win, "|");

    // text
    wmove(win, y+1, x+2);
    wprintw(win, function.c_str());
    wrefresh(win);

    wgetch(win);

    // TODO: to d/tor
    delwin(win);
}

void Gui::Screen::InitTerminal() {
    // start curses mode
    initscr();
    // disable line buffering
    cbreak();
    // don't print characters 
    noecho();
    // hide cursor
    curs_set(0);
    // clear the screen
    clear();
}

void Gui::Screen::EndTerminal() {
    // end ncurses
    endwin();
}
