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
    //std::cout << hp->EvalString("23 EEX 4 ENTER 3 +") << std::endl;
    //std::cout << Key::reverse_keys["EEX"] << std::endl;
#else
    auto b = Rpn::Backend(Key::keypad);
    std::cout << b.CalculateFromString("1337 STO 0 1 2 + RCL 0") << std::endl;;
#endif
}
