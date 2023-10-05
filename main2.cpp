#include "keypad.hpp" 
#include <iostream>

int main()
{
#if 1
    std::cout << std::get<0>(Key::keypad.single_arg_keys["l"])(10) << std::endl;

#endif
}
