#include "Region.h"

Region::Region(Pos dimensions): dimensions(dimensions) {
	assert(dimensions.x > 0 && dimensions.y > 0 && dimensions.z > 0);

	substance_grid.reset(new uint16_t[size()]());
	form_grid.reset(new Form[size()]());
	visibility_grid.reset(new bool[size()]());
	machine_grid.reset(new uint16_t[size()]());
}

bool Region::in_bounds(Pos pos) const {
	return pos.x >= 0 && pos.y >= 0 && pos.z >= 0 && pos.x < dim().x && pos.y < dim().y && pos.z < dim().z;
}

bool Region::in_bounds(Pos2 pos) const {
	return pos.x >= 0 && pos.y >= 0 && pos.x < dim().x && pos.y < dim().y;
}

Cell Region::get_cell(Pos pos) {
	int idx = this->idx(pos);
	return Cell(substance(idx), form(idx), visible(idx), machine(idx));
}

const Cell Region::get_cell(Pos pos) const {
	int idx = this->idx(pos);
	return Cell(substance_grid[idx], form_grid[idx], visibility_grid[idx], machine_grid[idx]);
}

bool Region::is_walkable(Pos pos) const {
	switch (form_grid[idx(pos)]) {
		case Form::Floor:
		case Form::Ramp:
			return true;
		case Form::Fill:
		case Form::Wall:
			return false;
		case Form::None: {
			const Cell& below = get_cell(pos - Pos(0, 0, 1));
			return below.form == Form::Fill || below.form == Form::Wall;
		}
	}
}
