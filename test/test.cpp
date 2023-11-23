#include "hip35.hpp"
#include <iostream>
#include <cassert>

#define ASSERT_FLOAT_CLOSE(actual, expected, epsilon) \
    do { \
        float actualValue = (actual); \
        float expectedValue = (expected); \
        float epsilonValue = (epsilon); \
        if (!(std::fabs(actualValue - expectedValue) < epsilonValue)) { \
            std::cerr << "Assertion failed: " #actual " and " #expected " not close at line " << __LINE__ << std::endl; \
        } \
    } while(0)

int main() {
	ASSERT_FLOAT_CLOSE(1, 1, 0.01);
	ASSERT_FLOAT_CLOSE(1, 2, 0.01);
	ASSERT_FLOAT_CLOSE(1, 1, 0.01);
	std::cout << "running tests\n";

}
