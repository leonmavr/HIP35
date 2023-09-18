#include "screen.hpp"
#include <utility> // make_pair
#include <ncurses.h>

Gui::Screen::Screen():
	active_key_(""),
    key_width_(12),
    key_height_(3)
{
    // state where each button is to be drawn
	InitKeypadGrid();
    // prepare the terminal for drawing 
    InitTerminal();
    DrawKeypad();
}

void Gui::Screen::SetUiDimensions() {
    max_width_pixels_ = 0;
    max_height_pixels_ = 0;
    for (const auto& pair: key_mappings_) {
        if (pair.second.second.x > max_width_pixels_)
            max_width_pixels_ = pair.second.second.x;
        if (pair.second.second.y > max_height_pixels_)
            max_height_pixels_ = pair.second.second.y;
    }
    // don't forget
    // -- increment by 1 (since grid counting starts from 0)
    // -- scale grid positions by width and height
    // -- increment by a small number to leave out some padding
    // to find UI width and height in pixels
    // width and height
    max_width_pixels_ += 1;
    max_height_pixels_ += 1;
    max_width_pixels_ *= key_width_;
    max_height_pixels_ *= key_height_;
    // leave out some padding
    max_width_pixels_ += 4;
    max_height_pixels_ += 4;

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
	key_mappings_["+"] = std::make_pair("+",     Gui::Point{0, 2});
	key_mappings_["-"] = std::make_pair("-",     Gui::Point{1, 2});
	key_mappings_["*"] = std::make_pair("*",     Gui::Point{2, 2});
	key_mappings_["/"] = std::make_pair("/",     Gui::Point{3, 2});
	key_mappings_["^"] = std::make_pair("^",     Gui::Point{4, 2});

	key_mappings_["s"] = std::make_pair("sin",   Gui::Point{0, 3});
	key_mappings_["c"] = std::make_pair("cos",   Gui::Point{1, 3});
	key_mappings_["t"] = std::make_pair("tan",   Gui::Point{2, 3});
	key_mappings_["S"] = std::make_pair("sqrt",  Gui::Point{3, 3});
	key_mappings_["l"] = std::make_pair("log",   Gui::Point{4, 3});

	key_mappings_["C"] = std::make_pair("chs",   Gui::Point{0, 4});
	key_mappings_["i"] = std::make_pair("inv",   Gui::Point{1, 4});
   
	// HP35 stack operations
	key_mappings_["v"] = std::make_pair("RDN",   Gui::Point{0, 5});
	key_mappings_["<"] = std::make_pair("SWAP",  Gui::Point{1, 5});
	key_mappings_["x"] = std::make_pair("LASTX", Gui::Point{2, 5});

    // records the dimensions of the UI in pixels
    SetUiDimensions();
}

bool Gui::Screen::DrawKey(const std::string& key, bool highlight) {
    bool found = false;
    const auto it = key_mappings_.find(key);
    if (it == key_mappings_.end())
        return found;

    // compile the text on the key, e.g.: l -> log (l)
    //                                    + -> +
    std::string text_on_key = key_mappings_[key].first;
    if (key != key_mappings_[key].first)
        text_on_key += " (" + key + ")";
    Point grid_pos = key_mappings_[key].second;
    // TODO: 4 -> screen y offset
    Point top_left_coords {grid_pos.x * Gui::Screen::key_width_ + 1,
                          4 + grid_pos.y * Gui::Screen::key_height_};
    // to remove horizontal whitespace between keys
    if (grid_pos.x != 0) top_left_coords.x -= grid_pos.x;
    // to remove vertical whitespace between keys
    if (grid_pos.y != 0) top_left_coords.y -= grid_pos.y;
    DrawBox(text_on_key, top_left_coords, highlight);

    found = true;
    return found;
}

void Gui::Screen::DrawBox(const std::string& text,
                          const Gui::Point& coords,
                          bool highlight) {
    // if we highlight just make it bold
    if (highlight)
        wattron(win_, A_BOLD);
    const int w = Gui::Screen::key_width_;
    const unsigned x = coords.x;
    const unsigned y = coords.y;
    // top left corner
    wmove(win_, y, x);
    wprintw(win_, "+");
    // top edge
    wmove(win_, y, x+1);
    whline(win_, '-', w-2);
    // top right corner
    wmove(win_, y, x+w-1);
    wprintw(win_, "+");
    // right edge
    wmove(win_, y+1, x+w-1);
    wprintw(win_, "|");
    // bottom right corner
    wmove(win_, y+2, x+w-1);
    wprintw(win_, "+");
    // bottom edge
    wmove(win_, y+2, x+1);
    whline(win_, '-', w-2);
    // bottom left corner
    wmove(win_, y+2, x);
    wprintw(win_, "+");
    // left edge
    wmove(win_, y+1, x);
    wprintw(win_, "|");

    // text
    wmove(win_, y+1, x+2);
    wprintw(win_, text.c_str());

    wrefresh(win_);
    // turn off highlighting
    if (highlight)
        wattroff(win_, A_BOLD);
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
    //const int nlines = Gui::Screen::key_height_ * 8;
    //const int ncols = Gui::Screen::key_width_ * 8;
    // NOTE: newwin call needs to be AFTER initscr()!
    const int startx = 0, starty = 0;
    win_ = newwin(max_height_pixels_, max_width_pixels_, startx, starty);
    //            l    r    t    d   tl   tr   bl   br
    wborder(win_, '.', '.', '.', '.', '.', '.', '.', '.');
}

void Gui::Screen::EndTerminal() {
    // deallocate ncurses window
    delwin(win_);
    // end ncurses
    endwin();
}

void Gui::Screen::DrawKeypad() {
    for (const auto& pair : key_mappings_) {
        DrawKey(pair.first);
    }
}
