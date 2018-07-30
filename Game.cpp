#include "Game.h"
#include "Region/Generator.h"
#include <iostream>

#ifdef NDEBUG
	#define SIZE 512
	#define HEIGHT 64
#else
	#define SIZE 256
	#define HEIGHT 32
#endif

Game::Game(): region(Pos(SIZE, SIZE, HEIGHT)), job_manager(region), unit_manager(region) {
	Generator generator;
	generator.generate(region);

	robit = &unit_manager.create_unit();
	unit_manager.spawn_unit(*robit, Pos2(region.dim().x / 2, region.dim().y / 2));
}

Menu& Game::get_menu() {
	return menu;
}

Region& Game::get_region() {
	return region;
}

UnitManager& Game::get_unit_manager() {
	return unit_manager;
}

void Game::tick(int amount) {
	unit_manager.tick(amount);
}

bool Game::is_paused() {
	return paused;
}
void Game::pause() {
	paused = true;
}
void Game::resume() {
	paused = false;
}

void Game::set_renderer(IRenderer& renderer) {
	this->renderer = &renderer;
	unit_manager.set_renderer(renderer);


	int build_menu = menu.add_submenu(0, menu.add_menu_option(0, "Build", 'b'));

	int wall_option = menu.add_menu_option(build_menu, "Wall",    'w');
	menu.add_callback(build_menu, wall_option, [&]() {
		renderer.get_selection([&](Pos pos) {
			Job& job = job_manager.create_build_job(pos, Form::Wall, 2);
			robit->assign_job(job);
		});
	});
	int floor_option = menu.add_menu_option(build_menu, "Floor",   'f');
	menu.add_callback(build_menu, floor_option, [&]() {
		renderer.get_selection([&](Pos pos) {
			Job& job = job_manager.create_build_job(pos, Form::Floor, 2);\
			robit->assign_job(job);
		});
	});
	menu.add_menu_option(build_menu, "Airlock", 'a');

	int debug_menu = menu.add_submenu(0, menu.add_menu_option(0, "Debug", 'd'));
	menu.add_menu_option(debug_menu, "Spawn Robit", 'r');

	menu.add_menu_option(0, "Quit", 'Q');
}
