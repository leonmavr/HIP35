#include "screen.hpp"
#include <utility> // make_pair
#include <iostream> // cout
#include <ncurses.h>

Gui::Screen::Screen():
	active_key_(""),
    key_width_(12),
    key_height_(3)
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

void Gui::Screen::DrawBox(const std::string& text,
                          const Gui::Point& coords,
                          bool highlight) {
    const int w = Gui::Screen::key_width_;
    const unsigned x = coords.x;
    const unsigned y = coords.y;
    const char edge_up_down = (highlight) ? '=' : '-';
    // top left corner
    wmove(win_, y, x);
    wprintw(win_, "+");
    // top edge
    wmove(win_, y, x+1);
    whline(win_, edge_up_down, w-2);
    // top right corner
    wmove(win_, y, x+w-1);
    wprintw(win_, "+");
    // right edge
    wmove(win_, y+1, x+w-1);
    wprintw(win_, "|");
    if (highlight) {
        wmove(win_, y+1, x+w-2);
        wprintw(win_, "|");
    }
    // bottom right corner
    wmove(win_, y+2, x+w-1);
    wprintw(win_, "+");
    // bottom edge
    wmove(win_, y+2, x+1);
    whline(win_, edge_up_down, w-2);
    // bottom left corner
    wmove(win_, y+2, x);
    wprintw(win_, "+");
    // left edge
    wmove(win_, y+1, x);
    wprintw(win_, "|");
    if (highlight) {
        wmove(win_, y+1, x+1);
        wprintw(win_, "|");
    }

    // text
    wmove(win_, y+1, x+2);
    wprintw(win_, text.c_str());
    wrefresh(win_);
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

    // initialize window where calculator is to be drawn
    const int nlines = Gui::Screen::key_height_ * 7;
    const int ncols = Gui::Screen::key_width_ * 8;
    // NOTE: newwin call needs to be AFTER initscr()!
    win_ = newwin(nlines, ncols, 0, 0);
    //            l    r    t    d   tl   tr   bl   br
    wborder(win_, '.', '.', '.', '.', '.', '.', '.', '.');
}

void Gui::Screen::EndTerminal() {
    // deallocate ncurses window
    delwin(win_);
    // end ncurses
    endwin();
}
