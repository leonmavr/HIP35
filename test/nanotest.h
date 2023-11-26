extern "C" {

#include <math.h>
#include <stdio.h>

#define NTEST_PASS 0
#define NTEST_FAIL 1
int ntest_result = NTEST_PASS;


#define NTEST_ASSERT_FLOAT_CLOSE(actual, expected) \
    do { \
        const float actual_value = (actual); \
        const float expected_value = (expected); \
        const float epsilon = 0.0001; \
        if (fabs(actual_value - expected_value) < epsilon) \
            printf("[v] Assertion at line %d passed.\n", __LINE__); \
        else { \
            fprintf(stderr, "[x] Assertion at line %d of file %s failed:\n" \
                            "    Values of %s and %s are not close\n", \
                    __LINE__, __FILE__, #actual, #expected); \
            ntest_result = NTEST_FAIL; \
        } \
    } while(0)


#define NTEST_ASSERT(condition) \
    do { \
        if (condition) \
            printf("[v] Assertion at line %d passed.\n", __LINE__); \
        else { \
            fprintf(stderr, "[x] Assertion at line %d of file %s failed" \
                            ":\n    %s", __LINE__, #condition); \
            ntest_result = NTEST_FAIL; \
        } \
    } while(0)


} // extern "C"
