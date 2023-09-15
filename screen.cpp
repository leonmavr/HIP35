#include "screen.hpp"
#include <utility> // make_pair
#include <ncurses.h>

Gui::Screen::Screen():
	active_key_(""),
    key_width_(12),
    key_height_(3)
{
    // prepare the terminal for drawing 
    InitTerminal();
    // state where each button is to be drawn
	InitKeypadGrid();
    DrawKeypad();
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
    //       text on key          text on key in ()      grid coords 
	key_mappings_["+"] =  std::make_pair("+",     Gui::Point{0, 2});
	key_mappings_["-"] =  std::make_pair("-",     Gui::Point{1, 2});
	key_mappings_["*"] =  std::make_pair("*",     Gui::Point{2, 2});
	key_mappings_["/"] =  std::make_pair("/",     Gui::Point{3, 2});
	key_mappings_["^"] =  std::make_pair("^",     Gui::Point{4, 2});

	key_mappings_["s"] =  std::make_pair("sin",   Gui::Point{0, 3});
	key_mappings_["c"] =  std::make_pair("cos",   Gui::Point{1, 3});
	key_mappings_["t"] =  std::make_pair("tan",   Gui::Point{2, 3});
	key_mappings_["S"] =  std::make_pair("sqrt",  Gui::Point{3, 3});
	key_mappings_["l"] =  std::make_pair("log",   Gui::Point{4, 3});

	key_mappings_["C"] =  std::make_pair("chs",   Gui::Point{0, 4});
	key_mappings_["i"] =  std::make_pair("inv",   Gui::Point{1, 4});
   
	// HP35 stack operations
	key_mappings_["v"] =  std::make_pair("RDN",   Gui::Point{0, 5});
	key_mappings_["<"] =  std::make_pair("SWAP",  Gui::Point{1, 5});
	key_mappings_["x"] =  std::make_pair("LASTX", Gui::Point{2, 5});
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
void Gui::Screen::HighlightKey(const std::string& key) {
    auto it = key_mappings_.find(key);
    if (it == key_mappings_.end())
        return;
    Point grid_position = key_mappings_[key].second; 
    int startx = grid_position.x * Gui::Screen::key_width_;
    int starty = grid_position.y * Gui::Screen::key_height_;
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
    // TODO: initialize based on grid and key dimensions
    const int nlines = Gui::Screen::key_height_ * 8;
    const int ncols = Gui::Screen::key_width_ * 8;
    // NOTE: newwin call needs to be AFTER initscr()!
    const int startx = 0, starty = 0;
    win_ = newwin(nlines, ncols, startx, starty);
    //            l    r    t    d   tl   tr   bl   br
    wborder(win_, '.', '.', '.', '.', '.', '.', '.', '.');

    // the window that highlights the currently pressed key
    win2_ = newwin(Gui::Screen::key_height_, Gui::Screen::key_width_, 0, 0);
}

void Gui::Screen::EndTerminal() {
    // deallocate ncurses window
    delwin(win_);
    // end ncurses
    endwin();
}

void Gui::Screen::DrawKeypad() {
    for (const auto& pair : key_mappings_) {
        std::string text = pair.second.first;
        if (pair.second.first != pair.first)
            text += " (" + pair.first + ")";
        Point grid_pos = pair.second.second;
        // TODO: 4 -> screen y offset
        Point text_coords{grid_pos.x * Gui::Screen::key_width_ + 1,
                          4 + grid_pos.y * Gui::Screen::key_height_};
        // to remove horizontal whitespace between keys
        if (grid_pos.x != 0) text_coords.x -= grid_pos.x;
        // to remove vertical whitespace between keys
        if (grid_pos.y != 0) text_coords.y -= grid_pos.y;
        DrawBox(text, text_coords);
    }
}
