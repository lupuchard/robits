#ifndef TOWN_GAME_H
#define TOWN_GAME_H

#include <unordered_set>
#include "Region/Region.h"
#include "JobManager.h"
#include "Menu.h"
#include "Unit.h"
#include "UnitManager.h"
#include "IRenderer.h"

class Game {
public:
	Game();

	void set_renderer(IRenderer& renderer);

	Menu& get_menu();
	Region& get_region();
	UnitManager& get_unit_manager();

	bool is_paused();
	void pause();
	void resume();

	void tick(int amount);

private:
	Menu menu;
	Region region;
	JobManager job_manager;
	UnitManager unit_manager;
	IRenderer* renderer = nullptr;
	Unit* robit = nullptr;
	bool paused = true;
};


#endif //TOWN_GAME_H
