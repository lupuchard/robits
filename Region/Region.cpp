#include <cassert>
#include "Region.h"

Region::Region(Pos dimensions): dimensions(dimensions) {
	assert(dimensions.x > 0 && dimensions.y > 0 && dimensions.z > 0);
	cells.reset(new Cell[dimensions.x * dimensions.y * dimensions.z]);
}

bool Region::in_bounds(Pos pos) const {
	return pos.x >= 0 && pos.y >= 0 && pos.z >= 0 && pos.x < dim().x && pos.y < dim().y && pos.z < dim().z;
}

bool Region::in_bounds(Pos2 pos) const {
	return pos.x >= 0 && pos.y >= 0 && pos.x < dim().x && pos.y < dim().y;
}

Pos Region::dim() const {
	return dimensions;
}

Cell& Region::get_cell(Pos pos) {
	assert(in_bounds(pos));
	return cells[pos.x + pos.y * dimensions.x + pos.z * dimensions.y * dimensions.x];
}

const Cell& Region::get_cell(Pos pos) const {
	assert(in_bounds(pos));
	return cells[pos.x + pos.y * dimensions.x + pos.z * dimensions.y * dimensions.x];
}

bool Region::is_walkable(Pos pos) const {
	assert(in_bounds(pos));
	const Cell& cell = get_cell(pos);

	switch (cell.form) {
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
