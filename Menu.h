#ifndef TOWN_MENU_H
#define TOWN_MENU_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

class Menu {
public:
	Menu();

	int current();

	int add_menu_option(int menu, std::string label, char shortcut = 0);
	void add_callback(int menu, int option, std::function<void()> callback);
	int num_options(int menu);

	int add_submenu(int menu, int option, std::string name = "");

	void trigger_select(int menu, int option);
	void trigger_select(int menu, char shortcut);
	void back_out();

	const std::string& get_label(int menu, int option);
	char get_shortcut(int menu, int option);

private:
	struct Option {
		Option(std::string label = "", char shortcut = 0):
			label(std::move(label)), shortcut(shortcut) { }
		std::string label;
		int submenu = -1;
		char shortcut;
		std::function<void()> callback;
	};
	struct Submenu {
		Submenu(std::string name = ""): name(std::move(name)) { }
		std::string name;
		std::vector<std::unique_ptr<Option>> options;
		std::unordered_map<char, int> shortcut_map;
		int parent = 0;
	};

	Submenu& get_menu(int menu_idx);
	Option& get_option(Submenu& menu, int option_idx);

	std::vector<std::unique_ptr<Submenu>> menus;
	int current_menu = 0;
};

#endif //TOWN_MENU_H
