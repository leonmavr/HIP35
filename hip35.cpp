#include "hip35.hpp"
#include "backend.hpp"
#include "screen.hpp"
#include "observer.hpp"
#include <memory> // unique_ptr

Hip35::Hip35() {
    backend_ = std::make_unique<Rpn::Backend>();
    frontend_ = std::make_unique<Gui::Frontend>();
    *observer_ = Observer();
    // convert unique pointer to regular pointer
    backend_->Attach(observer_.get());
}

static bool IsDecimal(const std::string& str) {
    try {
        size_t pos;
        std::stod(str, &pos);
        // Make sure the entire string was used for conversion
        return pos == str.length();
    } catch (const std::invalid_argument&) {
        return false;
    } catch (const std::out_of_range&) {
        return false;
    }
}

void Hip35::RunUI() {
    char input_char;
    std::string token = "";
    std::vector<std::string> functions = backend_->GetFunctions();
    bool previous_token_is_digit = false;
    while (1) {
        std::cin >> input_char;
        // check if it is space
        if (input_char == ' ') {
            // look at the token and decide if it's a number or function
            if (backend_->IsInFunctions(token)) {
                // process calculation stuff
            } else if (IsDecimal(token)) {
                // process insertion stuff
            }
            token = "";
        }
    }
}
