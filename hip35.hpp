#ifndef HIP35_HPP
#define HIP35_HPP 

#include "backend.hpp"
#include "screen.hpp"
#include "observer.hpp"
#include "keypad.hpp"
#include <memory>        // unique_ptr
#include <chrono>        // chrono::milliseconds
#include <string>        // string
#include <vector>        // vector
#include <unordered_map> // unordered_map 

namespace Ui {

class Hip35
{
public:
    Hip35();
    ~Hip35() { delete observer_; }
    double RunUI(bool run_headless = false);
    double EvalString(std::string expression);
    void SetDelay(unsigned ms) { delay_ms_ = std::chrono::milliseconds(ms); }

private:
    std::unique_ptr<Gui::Frontend> frontend_;
    std::unique_ptr<Rpn::Backend> backend_;
    Observer* observer_;
    // how many milliseconds to keep a button highlighted for after being pressed
    std::chrono::milliseconds delay_ms_;
    std::vector<std::string> tokens_no_ui_;
    const Key::Keypad& keypad_;
    // TODO: reverse keys involve code duplication, should find a better way
};

} // namespace Ui

#endif /* HIP35_HPP */
