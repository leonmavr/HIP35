extern "C" {

#include <math.h>
#include <stdio.h>

#define NTEST_PASS 0
#define NTEST_FAIL 1
int ntest_result = NTEST_PASS;

#define GET_FIRST(args) GET_FIRST_(args)
#define GET_FIRST_(N, ...) N 

// ## pastes the comma-separated variable arguments when present, 
// but removes the preceeding comma when they are not present
// based on @user1187902's: https://stackoverflow.com/a/16926582
#define COUNT_ARGS(...) COUNT_ARGS_(,##__VA_ARGS__, 6, 5, 4, 3, 2, 1, 0)
// for example with input (1, 3, 37) -> (empty is before , so)
// empty="", a=1, b=3, c=37, d=6, e=5, f=4, --> cnt=3
#define COUNT_ARGS_(empty, a, b, c, d, e, f, cnt, ...) cnt

// credits @p00ya (https://stackoverflow.com/a/62650578)
#define CHECK_EMPTY(default_val, ...) (default_val CHECK_EMPTY_(__VA_ARGS__))
#define CHECK_EMPTY_(...) ,##__VA_ARGS__

#define NTEST_ASSERT_FLOAT_CLOSE(...) NTEST_ASSERT_FLOAT_CLOSE_(__VA_ARGS__)
#define NTEST_ASSERT_FLOAT_CLOSE_(actual, expected, ...) \
    do { \
        const float actual_value = (actual); \
        const float expected_value = (expected); \
        float epsilon = 0.0001; \
        if (COUNT_ARGS(__VA_ARGS__) == 1) \
           epsilon = CHECK_EMPTY(epsilon, GET_FIRST(__VA_ARGS__)); \
        if (fabs(actual_value - expected_value) < epsilon) \
            printf("[v] Assertion at line %d passed.\n", __LINE__); \
        else { \
            fprintf(stderr, "[x] Assertion at line %d failed:\n    Values of %s and %s are not close\n", \
                    __LINE__, #actual, #expected); \
            ntest_result = NTEST_FAIL; \
        } \
    } while(0)


#define NTEST_ASSERT(condition) \
    do { \
        if (condition) \
            printf("[v] Assertion at line %d passed.\n", __LINE__); \
        else { \
            fprintf(stderr, "[x] Assertion at line %d failed:\n    %s", \
                    __LINE__, #condition); \
            ntest_result = NTEST_FAIL; \
        } \
    } while(0)

} // extern "C"
