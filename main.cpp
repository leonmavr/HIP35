#include "stack.hpp"
#include "backend.hpp"
#include "observer.hpp"
#include "screen.hpp"
#include "hip35.hpp"
#include "keypad.hpp"


int main() {
#if 0
    auto hp = Hip35();
    hp.RunUI();
    getchar();
#else
    auto b = Rpn::Backend(Key::keypad);
    std::cout << b.CalculateFromString("12 2 Ln LASTX ~10 +") << std::endl;;
#endif
}
