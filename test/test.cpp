#include "hip35.hpp"
#include "nanotest.h"

int main() {
    auto hp = std::make_unique<Ui::Hip35>(Key::keypad);
    NTEST_ASSERT_FLOAT_CLOSE(hp->EvalString("2 3 * CHS"), -6.0);
}
