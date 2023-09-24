#include "hip35.hpp"
#include "backend.hpp"
#include "screen.hpp"
#include "observer.hpp"
#include <memory> // unique_ptr

Hip35::Hip35(std::unique_ptr<Gui::Frontend> frontend,
             std::unique_ptr<Rpn::Backend> backend,
             std::unique_ptr<Observer> observer) {
    // convert unique pointer to regular pointer
    backend->Attach(observer.get());
}

void Hip35::RunUI() {
    char input_char;
    std::string token;
    std::vector<std::string> functions = backend_->GetFunctions();
    bool previous_token_is_digit = false;
    while (1) {
        std::cin >> input_char;
    }
}
