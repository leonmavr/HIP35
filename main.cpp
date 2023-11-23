#include "stack.hpp"
#include "backend.hpp"
#include "observer.hpp"
#include "screen.hpp"
#include "hip35.hpp"
#include "keypad.hpp"

#if 1
struct KeyInfoEex {
    std::function<void(Rpn::Backend&, std::optional<double>)> function;
    std::string annotation;
    Key::Point point;
    std::string long_key;
};

using idk = std::unordered_map<std::string, KeyInfoEex>;


#else

using idk = std::unordered_map<std::string, struct KeyInfoEex {
    std::string annotation;
    Key::Point point;
    std::string long_key;
};>;
#endif



int main() {
#if 0
    auto hp = std::make_unique<Ui::Hip35>(Key::keypad);
    hp->RunUI();
#else
    auto b = Rpn::Backend(Key::keypad);
    idk eex_key = {
        {
            Key::kKeyEex, KeyInfoEex{ 
            [](Rpn::Backend& b, std::optional<double> token) -> void { b.Eex(token); },
            "EEX",
            Key::Point{2, 4},
            "EEX"}
        }

    };
    auto it = eex_key.find(Key::kKeyEex);
    std::optional opt = std::optional<double>(5);
    b.Eex(opt);
    b.Eex(opt); // 500000
    std::cout << b << std::endl;
#endif
}
