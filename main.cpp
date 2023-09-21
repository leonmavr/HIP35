#include "stack.hpp"
#include "backend.hpp"
#include "observer.hpp"
#include "screen.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm> // find
#include <unistd.h> // usleep

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
    // TODO: wrap these in an HP35 class
    auto obs1 = Observer();
    auto rpn = std::make_unique<Rpn::Backend>();
    auto scr = std::make_unique<Gui::Frontend>();
    rpn->Attach(&obs1);
    std::string rpn_string = "2 5 * 4 + 3 2 * 1 + /";
    auto tokens = SplitStringBySpace(rpn_string);
    std::vector<std::string> functions = rpn->GetFunctions();
    // calculator needs enter to be pressed to separate two successive numbers
    bool previous_token_is_digit = false;
    for (const auto& token: tokens) {
        auto it = std::find(functions.begin(), functions.end(), token);
        if (it != functions.end()) {
            // function token found
            rpn->Calculate(token);
            previous_token_is_digit = false;
            scr->DrawKey(obs1.GetState().first, true);
            usleep(1000000); // in us
            scr->DrawKey(obs1.GetState().first);
        } else {
            if (previous_token_is_digit) {
                rpn->Enter();
                scr->DrawKey(obs1.GetState().first, true);
                usleep(1000000); // in us
                scr->DrawKey(obs1.GetState().first);
            }
            else {
                previous_token_is_digit = true;
            }
            rpn->Insert(std::stod(token));
        }
        scr->PrintRegisters(obs1.GetState().second.first, obs1.GetState().second.second);
    }
    getchar();
}
