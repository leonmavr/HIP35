#include "hip35.hpp"
#include "nanotest.h"
#include <iostream>

int main() {
    auto hp = std::make_unique<Ui::Hip35>(Key::keypad);
    // --pN shall mean page N from the manual:
    // http://h10032.www1.hp.com/ctg/Manual/c01579350

    NTEST_ASSERT_FLOAT_CLOSE(hp->EvalString("1 ENTER"),         1);

    //------------------------------------------------------------------//
    // 2-operand numeric                                                //
    //------------------------------------------------------------------//
    NTEST_ASSERT_FLOAT_CLOSE(hp->EvalString("12 ENTER 6 +"),    18);
    NTEST_ASSERT_FLOAT_CLOSE(hp->EvalString("12 ENTER 6 -"),    6);
    NTEST_ASSERT_FLOAT_CLOSE(hp->EvalString("12 ENTER 6 *"),    72);
    NTEST_ASSERT_FLOAT_CLOSE(hp->EvalString("12 ENTER 6 /"),    2);
    // (10-5)/((17-12)*4) = 0.25 -- p60
    NTEST_ASSERT_FLOAT_CLOSE(hp->EvalString(""
        "10 ENTER 5 - 17 ENTER 12 - 4 * /"),                    0.25);
    // bacterial growth -- p53:
    NTEST_ASSERT_FLOAT_CLOSE(hp->EvalString(""
        "1.5 ENTER ENTER ENTER 100 * * *"),                     337.5);

    //------------------------------------------------------------------//
    // 1-operand and 2-operand numeric                                  //
    //------------------------------------------------------------------//
    // sqrt((2+3)*(4+5)) + sqrt((6+7)*(8+9)) -- p60
    NTEST_ASSERT_FLOAT_CLOSE(hp->EvalString("2 ENTER 3 + 4 ENTER"
        " 5 + * SQRT 6 ENTER 7 + 8 ENTER 9 + * SQRT +"),        21.5743);
    constexpr float toler = 0.1;
    NTEST_ASSERT_FLOAT_CLOSE(hp->EvalString(""
        "10 LOG10 2.7 LN +"),                                   2, toler);
    NTEST_ASSERT_FLOAT_CLOSE(hp->EvalString("30 SIN"),          0.5);
    NTEST_ASSERT_FLOAT_CLOSE(hp->EvalString("0.5 ASIN"),        30);
    // final boss expression from the museum of HP35:
    // https://www.hpmuseum.org/35bkfin.jpg
    NTEST_ASSERT_FLOAT_CLOSE(hp->EvalString(""
        "2 ENTER 3 + 4 * 5 / 30 SIN / 1.5 CHS ENTER 4 ^ *"),    1);

    //------------------------------------------------------------------//
    // stack operations                                                 //
    //------------------------------------------------------------------//
    NTEST_ASSERT_FLOAT_CLOSE(hp->EvalString("13 ENTER 37 SWAP"), 13);
    NTEST_ASSERT_FLOAT_CLOSE(hp->EvalString(""
        "13 ENTER 37 RDN RDN RDN RDN"),                          37);
    NTEST_ASSERT_FLOAT_CLOSE(hp->EvalString("PI ENTER 1 +"),     4.14159);
    // sin(6) * tan(60/2)
    NTEST_ASSERT_FLOAT_CLOSE(hp->EvalString(""
        "60 SIN LASTX 2 / TAN *"),                               0.5);
    // --56 (three below)
    NTEST_ASSERT_FLOAT_CLOSE(hp->EvalString(""
        "16 ENTER 19 - LASTX + LASTX *"),                        16*19);
    NTEST_ASSERT_FLOAT_CLOSE(hp->EvalString(""
        "15 ENTER 19 * 16 LASTX *"),                             16*19);
    //------------------------------------------------------------------//
    // store/recall                                                     //
    //------------------------------------------------------------------//
    // NOTE:
    // store/recall do not work in EvalString yet. To see why,
    // read TODO comment in method's definition

    //------------------------------------------------------------------//
    // postfix exponent (EEX)                                           //
    //------------------------------------------------------------------//
    NTEST_ASSERT_FLOAT_CLOSE(hp->EvalString(""
        "2 EEX 3 ENTER 3 EEX 3 +"),                              5000);
    NTEST_ASSERT_FLOAT_CLOSE(hp->EvalString(""
        "2 EEX 3 ENTER 3 EEX 3 + 42 +"),                         5042);

    //------------------------------------------------------------------//
    // clear                                                            //
    //------------------------------------------------------------------//
    NTEST_ASSERT_FLOAT_CLOSE(hp->EvalString(""
        "123 CLX 2 ENTER 3 *"),                                   6);
}
