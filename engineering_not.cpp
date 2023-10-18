#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <cmath>

std::string FormatEngineeringNotation(double number, int precision) {
    // Find the order of magnitude (power of 10) of the number
    int orderOfMagnitude = static_cast<int>(std::floor(std::log10(std::fabs(number))));
    
    // Calculate the coefficient and the exponent
    double coefficient = number / std::pow(10, orderOfMagnitude);
    
    // Create a stringstream to format the number in engineering notation
    std::ostringstream coefficient_formatted;
    coefficient_formatted << std::fixed << std::setprecision(precision) << coefficient;// << " x 10^" << orderOfMagnitude;
    std::string coeff_string = coefficient_formatted.str();
    coeff_string.erase (coeff_string.find_last_not_of('0') + 1, std::string::npos ); 
    
    return coeff_string + "x10^" + std::to_string(orderOfMagnitude);
}

int main() {
    double number = 0.009234567;
    int precision = 2;
    std::string formattedNumber = FormatEngineeringNotation(number, precision);
    
    std::cout << "Formatted Number: " << formattedNumber << std::endl;
    
    return 0;
}

