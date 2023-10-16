#include "hip35.hpp"
#include "backend.hpp"
#include "screen.hpp"
#include "observer.hpp"
#include "keypad.hpp"
#include <memory> // unique_ptr

Hip35::Hip35():
        delay_ms_(std::chrono::milliseconds(100)) {
    backend_ = std::make_unique<Rpn::Backend>(Key::keypad);
    frontend_ = std::make_unique<Gui::Frontend>(Key::keypad);
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
    std::string operation = "";
    std::string operand = "";
    // if previous operation is STO (storage) / RCL (recall)
    // STO and RCL as prefix e.g. STO x, RCL so they are
    // treated differently
    bool is_prev_op_storage = false;
    while (1) {
        unsigned char c = getchar();
        std::string keypress = std::string(1, c);
        double regx = 0.0;
        double regy = 0.0;
        auto key_type = Rpn::kTypeNone;

        const auto it1 = Key::keypad.stack_keys.find(keypress);
                                        Key::keypad.stack_keys.end();
        const auto it2 = Key::keypad.single_arg_keys.find(keypress);
        const auto it3 = Key::keypad.double_arg_keys.find(keypress);
        const auto it4 = Key::keypad.storage_keys.find(keypress);

        if (keypress == Key::kKeyEnter)
            key_type = Rpn::kTypeEnter;
        else if (it1 != Key::keypad.stack_keys.end())
            key_type = Rpn::kTypeStack;
        else if ((it2 != Key::keypad.single_arg_keys.end()) ||
                  it3 != Key::keypad.double_arg_keys.end())
            key_type = Rpn::kTypeNumeric;
        else if (it4 != Key::keypad.storage_keys.end())
            key_type = Rpn::kTypeStorage;
        if (IsDecimal(operand + keypress)) {
            operand += keypress;
            key_type = Rpn::kTypeOperand;
        } else if (operand.empty() && (keypress == "~")) {
            operand = "-0";
            key_type = Rpn::kTypeOperand;
        }

        if (key_type == Rpn::kTypeNumeric) {
            // insert the operand the user was typing
            if (!operand.empty())
                backend_->Insert(std::stod(operand));
            // feed the keypress to backend to execute the function
            backend_->Calculate(keypress);
            // after execution, get registers X, Y and print them
            operation = observer_->GetState().first;
            regx = observer_->GetState().second.first; 
            regy = observer_->GetState().second.second; 
            frontend_->PrintRegisters(regx, regy);
            frontend_->HighlightKey(keypress, delay_ms_);
            // empty the operand to prepare for a new one
            operand = "";
            is_prev_op_storage = false;
        } else if (key_type == Rpn::kTypeStorage) {
            if (!operand.empty())
                backend_->Insert(std::stod(operand));
            // Storage/recall op/s are in prefix notation, e.g.
            // STO 2. Overwrite operation so that the loop knows
            // that it's expecting an argument to STO/RCL next.
            operation = keypress;
            regx = observer_->GetState().second.first; 
            regy = observer_->GetState().second.second; 
            frontend_->PrintRegisters(regx, regy);
            frontend_->HighlightKey(keypress, delay_ms_);
            operand = "";
            is_prev_op_storage = true;
        } else if (is_prev_op_storage) {
            try {
                const std::size_t idx = std::stoi(keypress);
                const auto it = Key::keypad.storage_keys.find(operation);
                std::get<0>(it->second)(*backend_, idx);
            } catch (const std::invalid_argument& e) {
                backend_->Cls();
            }
            operation = observer_->GetState().first;
            regx = observer_->GetState().second.first; 
            regy = observer_->GetState().second.second; 
            frontend_->PrintRegisters(regx, regy);
            frontend_->HighlightKey(keypress, delay_ms_);
            operand = "";
            is_prev_op_storage = false;
        } else if (keypress == Key::kKeyEnter) {
            // insert the currently typed opeand
            if (!operand.empty())
                backend_->Insert(std::stod(operand));
            // execute the ENTER function
            backend_->Enter();
            // get new values of registers and print them
            operation = observer_->GetState().first;
            regx = observer_->GetState().second.first; 
            regy = observer_->GetState().second.second; 
            frontend_->PrintRegisters(regx, regy);
            frontend_->HighlightKey(keypress, delay_ms_);
            // empty the operand to prepare for a new one
            operand = "";
            is_prev_op_storage = false;
        } else if (key_type == Rpn::kTypeStack) {
            // write currently typed number in the stack first
            if (!operand.empty())
                backend_->Insert(std::stod(operand));
            const auto it = Key::keypad.stack_keys.find(keypress);
            std::get<0>(it->second)(*backend_);
            operation = observer_->GetState().first;
            regx = observer_->GetState().second.first; 
            regy = observer_->GetState().second.second; 
            frontend_->PrintRegisters(regx, regy);
            frontend_->HighlightKey(keypress, delay_ms_);
            operand = "";
            is_prev_op_storage = false;
        } else if (key_type == Rpn::kTypeOperand) {
            operation = observer_->GetState().first;
            regx = observer_->GetState().second.first; 
            regy = observer_->GetState().second.second; 
            if (operation != Key::kKeyClx) {
                // We're about to insert to register X so display current
                // token at X as if the stack was lifted already
                frontend_->PrintRegisters(std::stod(operand), regx);
            }
            else {
                // The last operation cleared register X so we're
                // still writing in X. Y is left untouched
                frontend_->PrintRegisters(std::stod(operand), regy);
            }
            is_prev_op_storage = false;
        } else if (keypress == "q") {
            return;
        }
        if (is_prev_op_storage ||
            (key_type == Rpn::kTypeNumeric) ||
            (key_type == Rpn::kTypeStack) ||
            (key_type == Rpn::kTypeEnter) ||
            (key_type == Rpn::kTypeStack)) {

            // TODO
        }
    }
}
