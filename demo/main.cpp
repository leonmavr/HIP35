#include "hip35.hpp"
#include "keypad.hpp" // Key::keypad

int main() {
    auto hp = std::make_unique<Ui::Hip35>(key::keypad);
    hp->RunUI();
}
