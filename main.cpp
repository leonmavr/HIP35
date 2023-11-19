#include "stack.hpp"
#include "backend.hpp"
#include "observer.hpp"
#include "screen.hpp"
#include "hip35.hpp"
#include "keypad.hpp"


int main() {
#if 1
    auto hp = std::make_unique<Ui::Hip35>();
    hp->RunUI();
#else
    auto b = Rpn::Backend(Key::keypad);
    std::cout << b.CalculateFromString("1337 STO 0 1 2 + RCL 0") << std::endl;;
#endif
}
