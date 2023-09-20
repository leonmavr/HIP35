#include "stack.hpp"
#include "backend.hpp"
#include "observer.hpp"
#include "screen.hpp"
#include <iostream>
#include <sstream>
#include <string>

static std::vector<std::string> SplitStringBySpace(const std::string& string) {
    // space-delimited tokens to return
    std::vector<std::string> tokens;
    // space-delimited stream
    std::istringstream iss(string);
    while (iss) {
        std::string token;
        iss >> token;
        if (!token.empty())
            tokens.push_back(token);
    }
    return tokens;
}

int main() {
    auto obs1 = Observer();
    auto rpn = std::make_unique<Rpn::Backend>();
    rpn->Attach(&obs1);
    std::string rpn_string = "2 5 * 4 + 3 2 * 1 + /";
    auto tokens = SplitStringBySpace(rpn_string);
    // calculator needs enter to be pressed to separate two successive numbers
    bool press_enter = false;
    for (const auto& token: tokens) {
        
    }

#if 0
    // 2 5 * 4 + 3 2 * 1 + /
    rpn->Attach(&obs1);
    rpn->Insert(2);
    rpn->Enter();
    rpn->Insert(5);
    rpn->Calculate("*");
    rpn->Insert(4);
    rpn->Calculate("+");
    std::cout << "should be + " << obs1.GetState().first << std::endl;
    std::cout << "should some value " << obs1.GetState().second.first << obs1.GetState().second.second << std::endl;
    rpn->Insert(3);
    rpn->Enter();
    rpn->Insert(2);
    rpn->Calculate("*");
    rpn->Insert(1);
    rpn->Calculate("+");
    std::cout << rpn->Calculate("/") << std::endl; // 2
    std::cout << "---" << rpn->CalculateFromString("2 5 * 4 + 3 2 * 1 + /") << std::endl;
    //std::cout << "---" << rpn.calculateFromString("2 1.4 /") << std::endl;
    std::cout << *rpn << std::endl;
    std::cout << rpn->GetFunctions()[0] << std::endl;
    auto scr = Gui::Frontend();
    scr.DrawKey("+", true);
    scr.DrawKey("t", true);
    scr.DrawKey("t");
    scr.PrintRegisters(4.01, 20.0001);
    getchar();
#endif
}
