#include <cassert>
#include "UnitManager.h"

UnitManager::UnitManager(Region& region):
		path_finder(region), region(region) {
	unit_grid.reset(new std::vector<Unit*>[region.dim().x * region.dim().y]);
}

void UnitManager::tick(int amount) {
	for (auto& pair : units) {
		tick(*pair.second, amount);
	}
}

void UnitManager::tick(Unit& unit, int amount) {
	unit.overticks -= amount;
	while (unit.overticks < 0) {
		switch (unit.state) {
			case Unit::IDLE:
				unit.overticks = 0;
				break;
			case Unit::PENDING:
				switch (unit.current_job->get_type()) {
					case Job::Build:
						do_build(unit);
						break;
					default: break;
				}
				break;
			case Unit::TRAVEL: {
				if (unit.path_pos >= (int)unit.path.size()) {
					unit.state = Unit::PENDING;
					continue;
				}
				Pos diff = unit.path[unit.path_pos] - unit.pos;
				unit.overticks += 3 + (diff.x && diff.y) + ((bool)diff.z) * 2;
				move_unit(unit, unit.path[unit.path_pos]);
				unit.path_pos++;
			} break;
		}
	}
}

void UnitManager::do_build(Unit& unit) {
	BuildJob& build_job = *(BuildJob*)unit.current_job;

	Pos diff = build_job.site - unit.pos;
	if (diff.z == 0 && std::abs(diff.x) <= 1 && std::abs(diff.y) <= 1) {
		Cell& cell = region.get_cell(build_job.site);
		cell.form = build_job.form;
		cell.substance = build_job.substance;
		unit.state = Unit::IDLE;
		if (renderer) renderer->update_ground();
	} else {
		unit.path = path_finder.create_path(unit.pos, build_job.site);
		if (unit.path.empty()) {
			unit.state = Unit::IDLE;
			return;
		}
		unit.path.pop_back();
		unit.path_pos = 1;
		unit.state = Unit::TRAVEL;
	}
}

Unit& UnitManager::create_unit() {
	Unit* unit = new Unit(cur_unit_id);
	units[cur_unit_id].reset(unit);
	unit->pos.x = -1;
	cur_unit_id++;
	return *unit;
}

void UnitManager::move_unit(Unit& unit, Pos pos) {
	if (region.in_bounds(unit.pos)) {
		auto& vec = unit_grid[unit.pos.x + unit.pos.y * region.dim().x];
		for (size_t i = 0; i < vec.size(); i++) {
			if (vec[i] == &unit) {
				vec.erase(vec.begin() + i);
				break;
			}
		}
	}

	if (region.in_bounds(pos)) {
		auto& vec = unit_grid[pos.x + pos.y * region.dim().x];
		vec.push_back(&unit);
	}

	unit.pos = pos;
}

void UnitManager::spawn_unit(Unit& unit, Pos2 pos) {
	for (int i = region.dim().z - 1; i > 0; i--) {
		Pos p(pos.x, pos.y, (int16_t)i);
		if (region.is_walkable(p)) {
			move_unit(unit, p);
			return;
		}
	}
}

const std::vector<Unit*>& UnitManager::get_units(Pos2 pos) {
	assert(region.in_bounds(pos));
	return unit_grid[pos.x + pos.y * region.dim().x];
}

void UnitManager::set_renderer(IRenderer& renderer) {
	this->renderer = &renderer;
}
