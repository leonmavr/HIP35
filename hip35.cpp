#include "hip35.hpp"
#include "backend.hpp"
#include "screen.hpp"
#include "observer.hpp"
#include <memory> // unique_ptr

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
    while (1) {
        char input_char = getchar();
        const std::string input_char_str = std::string(1, input_char);
        if (backend_->IsInStackOperations((*frontend_)[input_char_str])) {
#if 1
            // write currently typed number in the stack first
            if (IsDecimal(token))
                backend_->Insert(std::stod(token));
            // discard current numerical token, then update it with the operation
            token = std::string(1, input_char);
            backend_->DoStackOperation(token);
            const double regx = observer_->GetState().second.first; 
            const double regy = observer_->GetState().second.second; 
            frontend_->PrintRegisters(regx, regy);
#endif
        } else if (backend_->IsInFunctions((*frontend_)[input_char_str])) {
            // if the user was typing a number, write it in the stack
            // before doing any calculations with it 
            if (IsDecimal(token))
                backend_->Insert(std::stod(token));
            // clear the number from the token
            token = std::string(1, input_char);
            // process calculation stuff
            backend_->Calculate((*frontend_)[input_char_str]);
            const double regx = observer_->GetState().second.first; 
            const double regy = observer_->GetState().second.second; 
            frontend_->PrintRegisters(regx, regy);
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
            // The symbol - is reserved for functions so use ~ as unary
            // minus. Then replace ~ with - for negative numbers.
            if (input_char == '~')
                token += "-0"; // -0 so IsDecimal returns true
            else
                token += input_char;
            if (IsDecimal(token)) {
                // we're about to write to register X so display current token
                // at register X and the current register X at register Y
                const double regx = observer_->GetState().second.first; 
                frontend_->PrintRegisters(std::stod(token), regx);
            } else {
                // invalid input so clear the stack (all 4 registers)
                backend_->Insert(0);
                backend_->Calculate("*");
                backend_->Calculate("*");
                backend_->Calculate("*");
                const double regx = observer_->GetState().second.first; 
                const double regy = observer_->GetState().second.second; 
                frontend_->PrintRegisters(regx, regy);
                token = "";
            }
            // else invalid input - clear all regs
        }
    }
}
