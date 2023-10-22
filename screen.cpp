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
#include <cfloat> // DBL_MIN 

namespace Gui {

Frontend::Frontend(const Key::Keypad& keypad):
    keypad_(keypad),
	active_key_(""),
    key_width_(12),
    key_height_(3),
    screen_height_(6),
    gen_reg_width_(12),
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
    for (const auto& pair: keypad_.stack_keys) {
        auto const& tuple = pair.second;
        const unsigned grid_x = std::get<2>(tuple).x;
        const unsigned grid_y = std::get<2>(tuple).y;
        if (grid_x > max_width_pixels_)
            max_width_pixels_ = grid_x;
        if (grid_y > max_height_pixels_)
            max_height_pixels_ = grid_y;
    }
    // don't forget
    // -- increment by 1 (to avoid drawing over the left border)
    // -- scale grid positions by width and height
    // -- increment by a small number to leave out some padding
    // to find UI width and height in pixels
    max_width_pixels_ += 1;
    max_height_pixels_ += 1;
    max_width_pixels_ *= key_width_;
    max_height_pixels_ *= key_height_;
    screen_width_ = max_width_pixels_ - 3;
    // offset by screen size
    max_height_pixels_ += screen_height_; 
    

    // make margin for general register area
    max_width_pixels_ += 3;
    constexpr unsigned offsety = 3;
    // this is the top left point where general register will be printed
    gen_regs_top_left_ = Key::Point{max_width_pixels_, offsety};
    for (int i = 0; i < Key::kNamesGenRegs.size(); i++)
        gen_regs_[Key::kNamesGenRegs[i]] = Key::Point{max_width_pixels_, offsety + 1};
    // make enough horizontal space for general register display
    max_width_pixels_ += gen_reg_width_;
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
    // records the dimensions of the UI in pixels
    SetUiDimensions();
    // so that ncurses knows it's not dealing with garbage dimension values
    dimensions_set_ = true;
}

bool Frontend::DrawKey(const std::string& key, bool highlight) {
    const auto& it1 = keypad_.stack_keys.find(key);
    const auto& it2 = keypad_.single_arg_keys.find(key);
    const auto& it3 = keypad_.double_arg_keys.find(key);
    const auto& it4 = keypad_.storage_keys.find(key);
    const bool found = (it1 != keypad_.stack_keys.end()) ||
                  (it2 != keypad_.single_arg_keys.end()) ||
                  (it3 != keypad_.double_arg_keys.end()) ||
                  (it4 != keypad_.storage_keys.end());
    // invalid input key - return without drawing anything
    if (!found)
        return found;

    Key::Point grid_pos{0, 0};
    std::string text_on_key = "";
    // search which map key belongs to and get the info from there
    if (it1 != keypad_.stack_keys.end()) {
        grid_pos = std::get<2>(it1->second);
        text_on_key = Key::AnnotateKey(
               it1, key);
    } else if (it2 != keypad_.single_arg_keys.end()) {
        grid_pos = std::get<2>(it2->second);
        text_on_key = Key::AnnotateKey(
               it2, key);
    } else if (it3 != keypad_.double_arg_keys.end()) {
        grid_pos = std::get<2>(it3->second);
        text_on_key = Key::AnnotateKey(
               it3, key);
    } else if (it4 != keypad_.storage_keys.end()) {
        grid_pos = std::get<2>(it4->second);
        text_on_key = Key::AnnotateKey(
               it4, key);
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
    for (const auto& pair : keypad_.stack_keys)
        DrawKey(pair.first);
    for (const auto& pair : keypad_.single_arg_keys)
        DrawKey(pair.first);
    for (const auto& pair : keypad_.double_arg_keys)
        DrawKey(pair.first);
    for (const auto& pair : keypad_.storage_keys)
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
    wmove(win_, 1, screen_width_ - 6);
    wprintw(win_, "HIP-35");
    wmove(win_, 3, 1);
    wprintw(win_, "Y");
    wmove(win_, 2, 2);
    whline(win_, '-', screen_width_ - 3);
    wmove(win_, 4, 1);
    wprintw(win_, "X");
    wmove(win_, 5, 2);
    whline(win_, '-', screen_width_ - 3);
    wmove(win_, 2, 2);
    wvline(win_, '|', 4);
    wmove(win_, 2, screen_width_ - 1);
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


static std::string FmtEngineeringNotation(double number, int precision) {
    // order of magnitude (power of 10) of the number
    const int magnitude = static_cast<int>(std::floor(
                              std::log10(std::fabs(number))));
    // the mantissa m (coefficient of the exponent)
    const double mantissa = number / std::pow(10, magnitude);
    // we will append the mantissa to an output string stream
    std::ostringstream mantissa_fmt;
    // then format it to the given precision
    mantissa_fmt << std::fixed << std::setprecision(precision)
                       << mantissa;
    std::string mantissa_string = mantissa_fmt.str();
    // erase any trailing zeros
    mantissa_string.erase(mantissa_string.find_last_not_of('0') + 1, 
                          std::string::npos); 
    std::string scientific_fmt = mantissa_string + " E" + std::to_string(magnitude);
    return scientific_fmt;
}

static std::string FmtFixedPrecision(double num, unsigned prec = 5) {
    std::ostringstream num_fmt;
    num_fmt << std::fixed << std::setprecision(prec) << num;
    auto num_fmt_string = num_fmt.str();
    // remove trailing zeros
    //num_fmt_string.erase(num_fmt_string.find_last_not_of('0') + 1, 
    //                     std::string::npos); 
    return num_fmt_string;
}

static inline std::string FmtBasedOnRange(double num, unsigned screen_width) {
    std::string ret = "";
    const unsigned nspaces = screen_width - 3;
    // below we pad with spaces to avoid having to redraw with ncurses
    if (std::abs(num) < DBL_MIN*100)
        ret = padString(FmtFixedPrecision(num, 5), nspaces);
    else  if (std::abs(num) < 0.01)
        ret = padString(FmtEngineeringNotation(num, 6), nspaces);
    else if (std::abs(num) < 1e3)
        ret = padString(FmtFixedPrecision(num, 5), nspaces);
    else if (std::abs(num) < 1e6)
        ret = padString(FmtFixedPrecision(num, 1), nspaces);
    else if (std::abs(num) < 1e12)
        ret = padString(FmtEngineeringNotation(num, 6), nspaces);
    else if (std::abs(num) < 1e18)
        ret = padString(FmtEngineeringNotation(num, 7), nspaces);
    else
        ret = padString(FmtEngineeringNotation(num, 8), nspaces);
    return ret;
}

bool Frontend::PrintRegisters(double regx, double regy) {
    // make sure that they're represented concisely on the screen
    // by choosing format based on their range
    std::string regx_str= FmtBasedOnRange(regx, screen_width_);
    std::string regy_str= FmtBasedOnRange(regy, screen_width_);
    // top screen row
    wmove(win_, 3, 3);
    wprintw(win_, regy_str.c_str());
    // bottom screen row
    wmove(win_, 4, 3);
    wprintw(win_, regx_str.c_str());
    wrefresh(win_);
    return dimensions_set_;
}

} // namespace Gui
