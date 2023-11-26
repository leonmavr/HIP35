#include "hip35.hpp"
#include "nanotest.h"

int main() {
    auto hp = std::make_unique<Ui::Hip35>(Key::keypad);
    NTEST_ASSERT_FLOAT_CLOSE(hp->EvalString("2 EEX 3 ENTER 3 EEX 3 +"),       5000);
    NTEST_ASSERT_FLOAT_CLOSE(hp->EvalString("2 EEX 3 ENTER 3 EEX 3 + 42 +"),  5042);
    NTEST_ASSERT_FLOAT_CLOSE(hp->EvalString("PI ENTER 1 +"),                  4.14159);
    NTEST_ASSERT_FLOAT_CLOSE(hp->EvalString("1 ENTER 2 SWAP CHS"),            -1);
    // manual p56: http://h10032.www1.hp.com/ctg/Manual/c01579350
    NTEST_ASSERT_FLOAT_CLOSE(hp->EvalString("16 ENTER 19 - LASTX + LASTX *"), 16*19);
    NTEST_ASSERT_FLOAT_CLOSE(hp->EvalString("15 ENTER 19 * 16 LASTX *"),      16*19);
    NTEST_ASSERT_FLOAT_CLOSE(hp->EvalString("16 ENTER 18 * LASTX / 19 *"),    16*19);
}
