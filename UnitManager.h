#ifndef TOWN_UNITMANAGER_H
#define TOWN_UNITMANAGER_H

#include "Region/Path.h"
#include "JobManager.h"
#include "Unit.h"
#include "IRenderer.h"

class UnitManager {
public:
	UnitManager(Region& region);

	void set_renderer(IRenderer& renderer);

	void tick(int amount);
	void tick(Unit& unit, int amount);

	Unit& create_unit();
	void move_unit(Unit& unit, Pos pos);
	void spawn_unit(Unit& unit, Pos2 pos);

	const std::vector<Unit*>& get_units(Pos2 pos);

private:
	void do_build(Unit& unit);

	PathFinder path_finder;
	Region& region;
	IRenderer* renderer = nullptr;

	int cur_unit_id = 1;
	std::unordered_map<int, std::unique_ptr<Unit>> units;
	std::unique_ptr<std::vector<Unit*>[]> unit_grid;
};


#endif //TOWN_UNITMANAGER_H
