#include "observer.hpp"

void Observer::UpdateOperation(const std::string& operation) {
    operation_ = operation;
}

void Observer::UpdateValue(double value) {
    // the last action wasn't an operation to make it empty
    operation_ = "";
    data_ = value;
};
