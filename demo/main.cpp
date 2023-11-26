#include "stack.hpp"
#include "backend.hpp"
#include "observer.hpp"
#include "frontend.hpp"
#include "hip35.hpp"
#include "keypad.hpp"

int main() {
    auto hp = std::make_unique<Ui::Hip35>(Key::keypad);
    hp->RunUI();
}
