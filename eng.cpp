#include <string>
#include <cmath>
#include <sstream>
#include <iostream>
#include <iomanip> // setprecision

std::string FormatEngineeringNotation(double number, int precision) {
    // order of magnitude (power of 10) of the number
    const int magnitude = static_cast<int>(std::floor(std::log10(std::fabs(number))));
    // the mantissa m (coefficient of the exponent)
    const double mantissa = number / std::pow(10, magnitude);
    // we will append the mantissa to an output string stream
    std::ostringstream mantissa_formatted;
    // then format it to the given precision
    mantissa_formatted << std::fixed << std::setprecision(precision)
                       << mantissa;
    std::string mantissa_string = mantissa_formatted.str();
    // erase any trailing zeros
    mantissa_string.erase(mantissa_string.find_last_not_of('0') + 1, 
                          std::string::npos); 
    std::string scientific_fmt = mantissa_string + " E" + std::to_string(magnitude);
    return scientific_fmt;
}

int main()
{
    std::cout << FormatEngineeringNotation(123.12345, 2) << std::endl;
}
