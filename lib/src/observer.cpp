#include "observer.hpp"

void Observer::UpdateOperation(const std::string& operation) {
    operation_ = operation;
}

void Observer::UpdateRegisters(std::pair<double, double> registers) {
    data_ = registers;
};
