#include "screen.hpp"

Gui::Screen::Screen(std::vector<std::string> keys):
	active_key_("") {
	MapTextToKey();
}

void Gui::Screen::MapTextToKey() {
	// arithmetic operations
	key_mappings_["log10"] = "l";
	key_mappings_["log"] = "l";
	key_mappings_["sin"] = "s";
	key_mappings_["cos"] = "c";
	key_mappings_["tan"] = "t";
	key_mappings_["sqrt"] = "S";
	key_mappings_["chs"] = "C";
	key_mappings_["inv"] = "i";
	key_mappings_["+"] = "+";
	key_mappings_["-"] = "-";
	key_mappings_["*"] = "*";
	key_mappings_["/"] = "/";
	key_mappings_["^"] = "^";
	// HP35 stack operations
	key_mappings_["RND"] = "v";
	key_mappings_["SWAP"] = "<";
	key_mappings_["LASTX"] = "x";
}
