#include "hip35.hpp"
#include "backend.hpp"
#include "screen.hpp"
#include "observer.hpp"
#include <memory> // unique_ptr
#include <iostream> // endl

Hip35::Hip35() {
    backend_ = std::make_unique<Rpn::Backend>();
    frontend_ = std::make_unique<Gui::Frontend>();
    observer_ = new Observer;
    // convert unique pointer to regular pointer
    backend_->Attach(observer_);
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
    std::string token = "";
    std::vector<std::string> functions = backend_->GetFunctions();
    bool previous_token_is_digit = false;
    while (1) {
        //std::cin.get(input_char);
        char input_char = getchar();
        // look at the token and decide if it's a number or function
#if 1
        // TODO: input_char frontend -> backend fn -> is in them?
        //
        const std::string input_char_str = std::string(1, input_char);
        if (backend_->IsInFunctions((*frontend_)[input_char_str])) {
            // clear the number from the token
            //token = std::string(1, input_char);
            // process calculation stuff
            //backend_->Calculate((*frontend_)[input_char_str]);
            //const auto regx = observer_->GetState().second.first; 
            //const auto regy = observer_->GetState().second.second; 
            //frontend_->PrintRegisters(regx, regy);
        } else if (input_char == ' '){
            // if the user was typing a number, write it in the stack
            // before pressing enter
            if (IsDecimal(token))
                backend_->Insert(std::stod(token));
            backend_->Enter();
            const double regx = observer_->GetState().second.first; 
            const double regy = observer_->GetState().second.second; 
            frontend_->PrintRegisters(regx, regy);
            token = "";
        } else if (input_char == 'q') {
            return;
        } else {
            // TODO: for negative numbers, use ~ as unary minus, since - is a function
            // so replace ~ with - here
            token += input_char;
            if (IsDecimal(token)) {
                // we're about to write to register X so display the current token
                // at register X and the existing register X at register Y
                const double regx = observer_->GetState().second.first; 
                frontend_->PrintRegisters(std::stod(token), regx);
            }
            // else throw exception and terminate
        }
        //std::cout << token << std::endl;
#endif
    }
}
