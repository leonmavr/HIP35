#ifndef SCREEN_HPP
#define SCREEN_HPP 

#include <unordered_map>
#include <string>
#include <vector>

namespace Gui {
class Screen
{
    public:
        Screen () = delete;
	// TODO
        Screen (std::vector<std::string> keys);
        ~Screen () {};
	// TODO
	void Draw();

    private:
	std::unordered_map<std::string, std::string> key_text_;
	std::unordered_map<std::string, std::string> key_mappings_;
	std::string active_key_;
	void MapTextToKey();
};

}

#endif /* SCREEN_HPP */
