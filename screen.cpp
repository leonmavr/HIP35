#include "screen.hpp"
#include <utility> // make_pair
#include <ncurses.h>

Gui::Screen::Screen(std::vector<std::string> keys):
	active_key_(""),
    key_width(9),
    key_height(1)
{
	InitKeypadGrid();
    // prepare the terminal for drawing 
    InitTerminal();
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

void Gui::Screen::DrawBox(const std::string& function, const std::string& key,
                          const Gui::Point& coords) {
    const unsigned x = coords.x;
    const unsigned y = coords.y;
    // TODO: set nlines and ncols from keypad dim/s
    const int nlines = 24;
    const int ncols = 50;
    // not sure if i'll need this
    // TODO: move to c/tor
    WINDOW * win = newwin(nlines, ncols, 0, 0);
    //box(win, 0, 0);
    wborder(win, 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H');
    wmove(win, y, x);
    wprintw(win, function.c_str());
    wrefresh(win);
#if 0
    // move cursor
    int row, col;       
    char mesg[]="Just a string";            /* message to be shown on the screen */

    getmaxyx(stdscr, row, col);             
    mvprintw(row/2,col/2-12,"%s", mesg);   
    getmaxyx(stdscr, row, col);               /* get the number of rows and columns */
    mvprintw(row-2, 0, "This screen has %d rows and %d columns\n", y, x);
    printw("Try resizing your window(if possible) and then run this program again");
    // render characters
    //refresh();
    // wait for key press 
#endif
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
