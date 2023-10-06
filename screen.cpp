#include "keypad.hpp"
#include "screen.hpp"
#include <utility> // make_pair, pair
#include <string> // to_string
#include <iostream> // cout 
#include <iomanip> // setprecision
#include <ncurses.h> // wrefresh, wprintw, etc.
#include <termios.h> // tcgetattr, tcsetattr
#include <unistd.h> // STDIN_FILENO
#include <chrono> // sleep_for, milliseconds
#include <thread> // this_thread

namespace Gui {

Frontend::Frontend(const Key::Keypad& keypad):
    keypad_(keypad),
	active_key_(""),
    key_width_(12),
    key_height_(3),
    screen_height_(6),
    dimensions_set_(false)
{
    // state where each button is to be drawn
	InitKeypadGrid();
    // prepare the terminal for drawing 
    InitTerminal();
    DrawKeypad();
    DrawScreen();
}

void Frontend::SetUiDimensions() {
    max_width_pixels_ = 0;
    max_height_pixels_ = 0;
    for (const auto& pair: key_mappings_) {
        if (pair.second.second.x > max_width_pixels_)
            max_width_pixels_ = pair.second.second.x;
        if (pair.second.second.y > max_height_pixels_)
            max_height_pixels_ = pair.second.second.y;
    }
    // don't forget
    // -- increment by 1 (to avoid drawing over the left border)
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
 *        Frontend
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
void Frontend::InitKeypadGrid() {
    // for the original positioning of the keys see:
    // https://upload.wikimedia.org/wikipedia/commons/3/34/HP-35_Red_Dot.jpg
    // Start from row 1 (y=1) since row 0 is reserved for the display 
    //     key to press    backend function to call    grid (x, y)
	key_mappings_["L"] = std::make_pair("log",   Point{0, 1});
	key_mappings_["l"] = std::make_pair("ln",    Point{1, 1});
	key_mappings_["e"] = std::make_pair("exp",   Point{2, 1});

	key_mappings_["S"] = std::make_pair("sqrt",  Point{0, 2});
	key_mappings_["s"] = std::make_pair("sin",   Point{1, 2});
	key_mappings_["c"] = std::make_pair("cos",   Point{2, 2});
	key_mappings_["t"] = std::make_pair("tan",   Point{3, 2});

	key_mappings_["i"] = std::make_pair("inv",   Point{0, 3});
	key_mappings_["<"] = std::make_pair("swap",  Point{1, 3});
	key_mappings_["v"] = std::make_pair("rdn",   Point{2, 3});
	key_mappings_["x"] = std::make_pair("lastx", Point{3, 3});
	key_mappings_["p"] = std::make_pair("pi",    Point{4, 3});

	key_mappings_["+"] = std::make_pair("+",     Point{0, 4});
	key_mappings_["-"] = std::make_pair("-",     Point{1, 4});
	key_mappings_["*"] = std::make_pair("*",     Point{2, 4});
	key_mappings_["/"] = std::make_pair("/",     Point{3, 4});
	key_mappings_["^"] = std::make_pair("^",     Point{4, 4});

	key_mappings_[" "] = std::make_pair("enter", Point{0, 5});
	key_mappings_["@"] = std::make_pair("clx",   Point{1, 5});
	key_mappings_["$"] = std::make_pair("cls",   Point{2, 5});
	key_mappings_["C"] = std::make_pair("chs",   Point{3, 5});
   
    // records the dimensions of the UI in pixels
    SetUiDimensions();
    // so that ncurses knows it's not dealing with garbage dimension values
    dimensions_set_ = true;
}

bool Frontend::DrawKey(const std::string& key, bool highlight) {
    const auto& it1 = keypad_.stack_keys.find(key);
    const auto& it2 = keypad_.single_arg_keys.find(key);
    const auto& it3 = keypad_.double_arg_keys.find(key);
    const bool found = (it1 != keypad_.stack_keys.end()) ||
                  (it2 != keypad_.single_arg_keys.end()) ||
                  (it3 != keypad_.double_arg_keys.end());
    // invalid input key - return without drawing anything
    if (!found)
        return found;

    Key::Point grid_pos{0, 0};
    std::string text_on_key = "";
    // search which map key belongs to and get the info from there
    if (it1 != keypad_.stack_keys.end()) {
        grid_pos = std::get<2>(it1->second);
        //text_on_key = Key::AnnotateKey(
         //       keypad_.stack_keys, key);
    } else if (it2 != keypad_.single_arg_keys.end()) {
        grid_pos = std::get<2>(it2->second);
        //text_on_key = Key::AnnotateKey(
         //       keypad_.single_arg_keys, key);
    } else if (it3 != keypad_.double_arg_keys.end()) {
        grid_pos = std::get<2>(it3->second);
        //text_on_key = Key::AnnotateKey(
         //       keypad_.double_arg_keys, key);
    }
    //Point grid_pos = key_mappings_[key].second;
    Point top_left_coords {grid_pos.x * Frontend::key_width_ + 1,
                          screen_height_ + grid_pos.y * Frontend::key_height_};
    // to remove horizontal whitespace between keys
    if (grid_pos.x != 0) top_left_coords.x -= grid_pos.x;
    // to remove vertical whitespace between keys
    if (grid_pos.y != 0) top_left_coords.y -= grid_pos.y;
    DrawBox(text_on_key, top_left_coords, highlight);
    return found;
}

bool Frontend::HighlightKey(const std::string& key,
                            std::chrono::milliseconds ms) {
    bool found = false;
    found = DrawKey(key, true);
    std::this_thread::sleep_for(ms);
    DrawKey(key);
    return found;
}

void Frontend::DrawBox(const std::string& text,
                          const Point& coords,
                          bool highlight) {
    // if we highlight just make it bold
    if (highlight)
        wattron(win_, A_BOLD);
    const int w = Frontend::key_width_;
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

    // text
    wmove(win_, y+1, x+2);
    wprintw(win_, text.c_str());

    wrefresh(win_);
    // turn off highlighting
    if (highlight)
        wattroff(win_, A_BOLD);
}

void Frontend::InitTerminal() {
    //// getchar modifications
    // modify terminal so that getchar reads character by character
    // without waiting to hit enter
    tcgetattr(STDIN_FILENO, &old_tio_);
    new_tio_ = old_tio_;
    // disable buffering for stdin (key press buffered automatically)
    new_tio_.c_lflag &=(~ICANON & ~ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio_);

    //// ncurses preparation
    // start curses mode
    initscr();
    // disable line buffering
    cbreak();
    // don't print characters 
    noecho();
    // hide cursor
    curs_set(0);
    // don't wait for Enter key press
    raw();
    // enable F keys
    keypad(stdscr, TRUE);
    // clear the screen
    clear();

    // initialize window where calculator is to be drawn
    // NOTE: newwin call needs to be AFTER initscr()!
    constexpr int startx = 0, starty = 0;
    win_ = newwin(max_height_pixels_, max_width_pixels_, startx, starty);
    //            l    r    t    d   tl   tr   bl   br
    wborder(win_, '.', '.', '.', '.', '.', '.', '.', '.');
}

void Frontend::EndTerminal() {
    // deallocate ncurses window
    delwin(win_);
    // end ncurses
    endwin();
    // Restore terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &old_tio_);
}

bool Frontend::DrawKeypad() {
    for (const auto& pair : key_mappings_)
        DrawKey(pair.first);
    return dimensions_set_; 
}

bool Frontend::DrawScreen() {
    /**
     * (0,0)
     * +-------------------------------------------+
     * |                                    HIP-35 |
     * | |----------------------------------------||
     * |Y|                                        ||
     * |X|                                        ||
     * | |----------------------------------------||
     * |                                           |
     */
    // wmove(win_name, y, x)
    wmove(win_, 1, max_width_pixels_ - 7);
    wprintw(win_, "HIP-35");
    wmove(win_, 3, 1);
    wprintw(win_, "Y");
    wmove(win_, 2, 2);
    whline(win_, '-', max_width_pixels_ - 4);
    wmove(win_, 4, 1);
    wprintw(win_, "X");
    wmove(win_, 5, 2);
    whline(win_, '-', max_width_pixels_ - 4);
    wmove(win_, 2, 2);
    wvline(win_, '|', 4);
    wmove(win_, 2, max_width_pixels_ - 2);
    wvline(win_, '|', 4);
    wrefresh(win_);
    return dimensions_set_;
}

static std::string padString(const std::string& input, std::size_t N) {
    if (input.length() >= N) {
        // No need to pad, return the original string
        return input;
    } else {
        // Calculate the number of spaces needed
        std::size_t spacesToAdd = N - input.length();
        // Create a string of spaces
        std::string padding(spacesToAdd, ' ');
        // Concatenate the padding and the input string
        return padding + input;
    }
}

bool Frontend::PrintRegisters(double regx, double regy) {
    const unsigned screen_width = max_width_pixels_ - 4;
    std::string regx_string = std::to_string(regx);
    std::string regy_string = std::to_string(regy);
    regx_string = padString(regx_string, screen_width-2);
    regy_string = padString(regy_string, screen_width-2);
    // top screen row
    wmove(win_, 3, 3);
    wprintw(win_, regy_string.c_str());
    // bottom screen row
    wmove(win_, 4, 3);
    wprintw(win_, regx_string.c_str());
    wrefresh(win_);
    return dimensions_set_;
}

} // namespace Gui
