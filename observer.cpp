#include "observer.hpp"

void Observer::UpdateOperation(const std::string& operation) {
    operation_ = operation;
}

void Observer::UpdateRegisters(std::pair<double, double> registers) {
    // the last action wasn't an operation to make it empty
    operation_ = "";
    data_ = registers;
};
