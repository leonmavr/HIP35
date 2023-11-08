#ifndef HIP35_HPP
#define HIP35_HPP 

#include "backend.hpp"
#include "screen.hpp"
#include "observer.hpp"
#include <memory> // unique_ptr
#include <chrono> // chrono::milliseconds

namespace Ui {

class Hip35
{
public:
    Hip35();
    ~Hip35() { delete observer_; }
    double RunUI();
    void SetDelay(unsigned ms) { delay_ms_ = std::chrono::milliseconds(ms); }
    void HeadlessMode() { headless_mode = true; };

private:
    std::unique_ptr<Gui::Frontend> frontend_;
    std::unique_ptr<Rpn::Backend> backend_;
    Observer* observer_;
    // how many milliseconds to keep a button highlighted for after being pressed
    std::chrono::milliseconds delay_ms_;
    bool headless_mode;

};

} // namespace Ui

#endif /* HIP35_HPP */
