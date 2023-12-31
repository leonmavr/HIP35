#include "hip35.hpp"
#include "backend.hpp"
#include "frontend.hpp"
#include "observer.hpp"
#include "keypad.hpp"
#include <memory>       // unique_ptr
#include <sstream>      // std::stringstream


namespace Ui {

Hip35::Hip35(const key::Keypad& keypad):
        delay_ms_(std::chrono::milliseconds(100)),
        tokens_no_ui_(),
        keypad_(keypad) {
    backend_ = std::make_unique<backend::Backend>(keypad_);
    frontend_ = std::make_unique<gui::Frontend>(keypad_);
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


double Hip35::RunUI(bool run_headless) {
    std::string operation = "";
    std::string operand = "";
    // if previous operation is STO (storage) / RCL (recall)
    // STO and RCL as prefix e.g. STO x, RCL so they are
    // treated differently
    bool is_prev_op_storage = false;
    // close the ncurses window if set so 
    if (run_headless)
        frontend_->CloseUi();

    while (1) {
        std::string keypress = "";
        if (!run_headless) {
            unsigned char c = getchar();
            keypress = std::string(1, c);
        } else {
            if (tokens_no_ui_.empty())
                break;
            keypress = tokens_no_ui_.front();
            tokens_no_ui_.erase(tokens_no_ui_.begin());
        }
        double regx = 0.0;
        double regy = 0.0;
        auto key_type = backend::kTypeNone;
        //------------------------------------------------------
        // Determine operation type
        //------------------------------------------------------
        const auto it1 = keypad_.stack_keys.find(keypress);
                                        keypad_.stack_keys.end();
        const auto it2 = keypad_.single_arg_keys.find(keypress);
        const auto it3 = keypad_.double_arg_keys.find(keypress);
        const auto it4 = keypad_.storage_keys.find(keypress);

        if (keypress == key::kKeyEnter)
            key_type = backend::kTypeEnter;
        else if (it1 != keypad_.stack_keys.end())
            key_type = backend::kTypeStack;
        else if ((it2 != keypad_.single_arg_keys.end()) ||
                  it3 != keypad_.double_arg_keys.end())
            key_type = backend::kTypeNumeric;
        else if (it4 != keypad_.storage_keys.end())
            key_type = backend::kTypeStorage;
        //------------------------------------------------------
        // Append to operand if necessary 
        //------------------------------------------------------
        if (IsDecimal(operand + keypress)) {
            operand += keypress;
            key_type = backend::kTypeOperand;
        } else if (operand.empty() && (keypress == "~")) {
            operand = "-0";
            key_type = backend::kTypeOperand;
        }

        auto PrintRegs = [&]() {
            if (!run_headless) {
                regx = observer_->GetState().second.first; 
                regy = observer_->GetState().second.second; 
                frontend_->PrintRegisters(regx, regy);
                frontend_->HighlightKey(keypress, delay_ms_);
            }
        };

        //------------------------------------------------------
        // Call Backend instance to execute 
        //------------------------------------------------------
        if (keypress == key::kKeyEex) {
            // it can be unset (empty operand) or a decimal
            std::optional<double> opt_operand;
            if (IsDecimal(operand))
                opt_operand = std::stod(operand);
            backend_->Eex(opt_operand);
            PrintRegs();
            operand = "";
            is_prev_op_storage = false;
        } else if (is_prev_op_storage) {
            // check this first as storage may use the same keys
            // as the keypad functions
            try {
                const auto it = keypad_.storage_keys.find(operation);
                (it->second.function)(*backend_, keypress);
            } catch (const std::invalid_argument& e) {
                // don't do anything and wait for next key
            }
            operation = observer_->GetState().first;
            if (operation == key::kKeyStore) {
                const double regx = observer_->GetState().second.first;
                frontend_->PrintGenRegister(keypress, regx);
            } else if (operation == key::kKeyRcl) {
                // registers have changed due to RCL
                PrintRegs();
            }
            operand = "";
            is_prev_op_storage = false;
        } else if (key_type == backend::kTypeNumeric) {
            // write currently typed number in the stack first
            if (!operand.empty())
                backend_->Insert(std::stod(operand));
            // feed the keypress to backend to execute the function
            backend_->Calculate(keypress);
            // store operation and print registers after execution
            operation = observer_->GetState().first;
            PrintRegs();
            // empty the operand to prepare for a new one
            operand = "";
            is_prev_op_storage = false;
        } else if (key_type == backend::kTypeStorage) {
            if (!operand.empty())
                backend_->Insert(std::stod(operand));
            // Storage/recall op/s are in prefix notation, e.g.
            // STO 2. Overwrite operation so that the loop knows
            // that it's expecting an argument to STO/RCL next.
            operation = keypress;
            PrintRegs();
            operand = "";
            is_prev_op_storage = true;
        } else if (keypress == key::kKeyEnter) {
            if (!operand.empty())
                backend_->Insert(std::stod(operand));
            backend_->Enter();
            operation = observer_->GetState().first;
            PrintRegs();
            operand = "";
            is_prev_op_storage = false;
        } else if (key_type == backend::kTypeStack) {
            if (!operand.empty())
                backend_->Insert(std::stod(operand));
            const auto it = keypad_.stack_keys.find(keypress);
            (it->second.function)(*backend_);
            operation = observer_->GetState().first;
            PrintRegs();
            operand = "";
            is_prev_op_storage = false;
        } else if (key_type == backend::kTypeOperand) {
            operation = observer_->GetState().first;
            regx = observer_->GetState().second.first; 
            regy = observer_->GetState().second.second;
            if (operation == key::kKeyEex) {
                // EEX was pressed - show the result for the curently typed operand
                if (!run_headless)
                    frontend_->PrintRegisters(std::pow(10, std::stod(operand)) * regx, regy);
            } else if (operation != key::kKeyClx) {
                // We're about to insert to register X so display current
                // token at X as if the stack was lifted already
                if (!run_headless)
                    frontend_->PrintRegisters(std::stod(operand), regx);
            }
            else {
                // The last operation cleared register X so we're
                // still writing in X. Y is left untouched
                if (!run_headless)
                    frontend_->PrintRegisters(std::stod(operand), regy);
            }
            is_prev_op_storage = false;
        } else if (keypress == "q") {
            break;
        }
    }
    const auto regx = observer_->GetState().second.first;
    return regx; 
}

double Hip35::EvalString(std::string expression) {
    // clear the buffer to prepare it for new expression
    tokens_no_ui_.clear();
    std::istringstream iss(expression);
    std::string token;
    while (std::getline(iss, token, ' ')) {
        const auto it = keypad_.reverse_keys.find(token);
        // then the user has entered a long key for an operation,
        // e.g. LOG10 and we reverse it to L
        if (it != keypad_.reverse_keys.end()) {
            const auto short_key = it->second;
            tokens_no_ui_.push_back(short_key);
        }
        else {
            // operand
            tokens_no_ui_.push_back(token);
        }
    }
    constexpr bool headless = true;
    return RunUI(headless);
}

} // namespace Ui
