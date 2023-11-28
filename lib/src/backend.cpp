#include "observer.hpp"
#include "backend.hpp"
#include "stack.hpp"
#include "keypad.hpp"
#include <iostream> // ostream
#include <iomanip> // setprecision, fixed
#include <vector> // vector 
#include <sstream> // istringstream
#include <stdexcept> // runtime_error 
#include <algorithm> // erase, remove
#include <cmath> // M_PI
#include <cfloat> // DBL_MIN 
#include <optional> // optional 


void Subject::Detach(Observer* observer) {
    observers_.erase(
        std::remove(observers_.begin(), observers_.end(), observer),
        observers_.end()
    );
} 

void Subject::NotifyValue(std::pair<double, double> registers) {
    for (const auto& observer : observers_)
        observer->UpdateRegisters(registers);
}

void Subject::NotifyOperation(const std::string& operation) {
    for (const auto& observer : observers_)
        observer->UpdateOperation(operation);
}

namespace backend {

Backend::Backend(const Key::Keypad& keypad):
    keypad_(keypad),
    stack_(std::make_unique<Stack>()),
    lastx_(0.0),
    sto_regs_({0})
{ 
    // map a general register name to the index of the gr array
    std::size_t i = 0;
    for (const auto& n: Key::kNamesGenRegs)
        gen_regs_name2idx[n] = i++;
    // initialize flags
    flags_.shift_up = true;
    flags_.eex_pressed = false;
    flags_.rcl_sto_pressed = false;
}

void Backend::Rdn() {
    // we always use the stack pointer because Stack class implements a [] operator
    auto old_first = (*stack_)[0];
    for (std::size_t i = 0; i < (*stack_).size() - 1; ++i)
        (*stack_)[i] = (*stack_)[i+1];
    (*stack_)[(*stack_).size() - 1] = old_first;
    flags_.eex_pressed = false;
    // inform the observer
    NotifyValue(Peek());
    NotifyOperation(Key::kKeyRdn);
}

void Backend::SwapXY() {
    std::swap((*stack_)[IDX_REG_X], (*stack_)[IDX_REG_Y]);
    flags_.eex_pressed = false;
    // inform the observer
    NotifyValue(Peek());
    NotifyOperation(Key::kKeySwap);
}

void Backend::Insert(double num) {
    if (flags_.eex_pressed) {    
        (*stack_)[IDX_REG_X] *= std::pow(10, num);
    } else if (flags_.shift_up) { // number was entered
        stack_->ShiftUp();
        stack_->writeX(num);
    } else { // Enter was pressed so write in current reg. X
        stack_->writeX(num);
    }
    flags_.shift_up = true;
    flags_.eex_pressed = false;
    // notify class observers about new value
    NotifyValue(Peek());
}

void Backend::Enter() {
    stack_->ShiftUp();
    (*stack_)[IDX_REG_X] = (*stack_)[IDX_REG_Y];
    flags_.eex_pressed = false;
    flags_.shift_up = false;
    // notify class observer since enter manipulates the stack
    NotifyValue(Peek());
    // don't forget to notify the observer so we can use the event later
    NotifyOperation(Key::kKeyEnter);
}

void Backend::LastX() {
    // Make space to insert regisrer LASTX
    stack_->ShiftUp();
    (*stack_)[IDX_REG_X] = lastx_;
    flags_.eex_pressed = false;
    // inform the observer
    NotifyValue(Peek());
    NotifyOperation(Key::kKeyLastX);
}

double Backend::Calculate(std::string operation) {
    flags_.shift_up = true;
    auto& registerX = (*stack_)[IDX_REG_X];
    auto& registerY = (*stack_)[IDX_REG_Y];
    // We did an operation so calculator needs to store register X
    // before the operation in register LASTX
    lastx_ = registerX;
    bool valid_operation = false;

    auto it1 = keypad_.single_arg_keys.find(operation);
    if (it1 != keypad_.single_arg_keys.end()) {
        // query single operand op/s such as sin, log, etc.
        //registerX  = function_key_1op_[operation](registerX);
        registerX = (it1->second.function)(registerX);
        valid_operation = true;
    }
    auto it2 = keypad_.double_arg_keys.find(operation);
    if (it2 != keypad_.double_arg_keys.end()) {
        // query 2-operant operations such as +, /, etc.
        registerY = (it2->second.function)(registerX, registerY);
        // drop old register X
        stack_->ShiftDown();
        valid_operation = true;
    }
    if (valid_operation)
    {
        // Notify observers about the new operation and value
        NotifyOperation(operation); 
        NotifyValue(Peek()); 
        return registerX;
    } else {
        throw std::runtime_error(std::string("[FATAL]: Invalid operation ") +
                                            operation + std::string("\n"));
    }
}

void Backend::Clx() {
    stack_->writeX(0.0);
    flags_.shift_up = false;
    // inform the observer 
    NotifyOperation(Key::kKeyClx); 
    NotifyValue(Peek()); 
}

void Backend::Cls() {
    stack_->writeX(0.0);
    Enter();
    Enter();
    Enter();
    NotifyOperation(Key::kKeyCls); 
    NotifyValue(Peek()); 
}

void Backend::Pi() {
    flags_.eex_pressed = false;
    Insert(M_PI);
    // inform the observer 
    NotifyOperation(Key::kKeyPi); 
    NotifyValue(Peek()); 
}

static inline bool IsNearZero(double x) {
    return std::fabs(x) < DBL_MIN*100;
}

void Backend::Eex(std::optional<double> token) {
    const double regx = Peek().first; 
    if (IsNearZero(*token) && IsNearZero(regx)) // prepare register X
        stack_->writeX(1);
    else if (flags_.eex_pressed) // multiply consecutively
        (*stack_)[IDX_REG_X] *= std::pow(10, *token);
    else if (IsNearZero(*token) && !IsNearZero(regx))
        ; // don't do anything
    else
        stack_->writeX(*token);
    flags_.shift_up = false;
    flags_.eex_pressed = true;
    NotifyOperation(Key::kKeyEex); 
    NotifyValue(Peek()); 
}

static inline std::string ToUpper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s;
}

static inline std::string ToLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

void Backend::Sto(std::string name) {
    // general register index
    std::size_t idx;
    // name is case insensitive
    if (gen_regs_name2idx.find(ToUpper(name)) != gen_regs_name2idx.end())
        idx = gen_regs_name2idx[ToUpper(name)];
    else if (gen_regs_name2idx.find(ToLower(name)) != gen_regs_name2idx.end())
        idx = gen_regs_name2idx[ToLower(name)];
    else // silently ignore index errors
        return;

    if (idx < sto_regs_.size())
        sto_regs_[idx] = (*stack_)[IDX_REG_X];
    flags_.shift_up = true;
    flags_.eex_pressed = false;

    NotifyOperation(Key::kKeyStore); 
    // doesn't change the stack so no values sent to observer
}

void Backend::Rcl(std::string name) {
    // general register index
    std::size_t idx;
    // name is case insensitive - check validity
    if (gen_regs_name2idx.find(ToUpper(name)) != gen_regs_name2idx.end())
        idx = gen_regs_name2idx[ToUpper(name)];
    else if (gen_regs_name2idx.find(ToLower(name)) != gen_regs_name2idx.end())
        idx = gen_regs_name2idx[ToLower(name)];
    else // silently ignore index errors
        return;

    // RCL operation stores X in LASTX:
    // http://h10032.www1.hp.com/ctg/Manual/c01579350 p306
    lastx_ = (*stack_)[IDX_REG_X];
    // silently ignore index errors
    if (idx < sto_regs_.size())
        (*stack_)[IDX_REG_X] = sto_regs_[idx];
    flags_.shift_up = true;
    flags_.eex_pressed = false;
    NotifyOperation(Key::kKeyRcl); 
    NotifyValue(Peek()); 
}


std::ostream& operator<<(std::ostream& os, const Backend& backend) {
    const auto& stack = *(backend.stack_);
    os << std::fixed << std::setprecision(2) <<
        "X\tY\tZ\tT\tLASTX" << std::endl <<
        stack[IDX_REG_X] << "\t" <<
        stack[IDX_REG_Y] << "\t" <<
        stack[IDX_REG_Z] << "\t" <<
        stack[IDX_REG_T] << "\t" <<
        backend.lastx_ << std::endl;
    return os;
}

} /* namespace backend */
