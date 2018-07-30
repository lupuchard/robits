#include <utility>

#include "Menu.h"
#include <cassert>

Menu::Menu() {
	menus.emplace_back(new Submenu());
	menus.back()->name = "Menu";
}

int Menu::current() {
	return current_menu;
}


Menu::Submenu& Menu::get_menu(int menu_idx) {
	assert(menu_idx >= 0 && menu_idx < (int)menus.size());
	return *menus[menu_idx];
}

Menu::Option& Menu::get_option(Menu::Submenu& menu, int option_idx) {
	assert(option_idx >= 0 && option_idx < (int)menu.options.size());
	return *menu.options[option_idx];
}

int Menu::add_menu_option(int menu_idx, std::string label, char shortcut) {
	Submenu& menu = get_menu(menu_idx);
	menu.options.emplace_back(new Option(std::move(label), shortcut));
	int option_idx = (int)(menu.options.size() - 1);
	if (shortcut) {
		menu.shortcut_map[shortcut] = option_idx;
	}
	return option_idx;
}
int Menu::num_options(int menu_idx) {
	return (int)get_menu(menu_idx).options.size();
}

void Menu::add_callback(int menu_idx, int option_idx, std::function<void()> callback) {
	Option& option = get_option(get_menu(menu_idx), option_idx);
	option.callback = callback;
}

int Menu::add_submenu(int menu_idx, int option_idx, std::string name) {
	Option& option = get_option(get_menu(menu_idx), option_idx);
	menus.emplace_back(new Submenu());
	option.submenu = (int)(menus.size() - 1);
	menus.back()->name = name.empty() ? option.label : name;
	menus.back()->parent = menu_idx;
	return option.submenu;
}

void Menu::trigger_select(int menu_idx, int option_idx) {
	Option& option = get_option(get_menu(menu_idx), option_idx);
	if (option.submenu != -1) {
		current_menu = option.submenu;
	}
	if (option.callback) {
		option.callback();
	}
}

void Menu::trigger_select(int menu_idx, char shortcut) {
	Submenu& menu = get_menu(menu_idx);
	auto iter = menu.shortcut_map.find(shortcut);
	if (iter != menu.shortcut_map.end()) {
		trigger_select(menu_idx, iter->second);
	}
}

void Menu::back_out() {
	current_menu = menus[current_menu]->parent;
}

const std::string& Menu::get_label(int menu_idx, int option_idx) {
	return get_option(get_menu(menu_idx), option_idx).label;
}

char Menu::get_shortcut(int menu_idx, int option_idx) {
	return get_option(get_menu(menu_idx), option_idx).shortcut;
}
