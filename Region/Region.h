#ifndef UNTITLED_REGION_H
#define UNTITLED_REGION_H

#include <vector>
#include <memory>
#include <cassert>
#include "Pos.h"

enum class Form: uint8_t {
	None,
	Fill,
	Ramp,
	Wall,
	Floor
};


struct Cell {
	Cell(uint16_t& substance, Form& form, bool& visible, uint16_t& machine):
		substance(substance), form(form), visible(visible), machine(machine) { }
	uint16_t& substance;
	Form& form;
	bool& visible;
	uint16_t& machine;
};

class Region {
public:
	explicit Region(Pos dimensions);

	inline Pos dim() const { return dimensions; }
	inline int size() const { return dimensions.x * dimensions.y * dimensions.z; }
	bool in_bounds(Pos pos) const;
	bool in_bounds(Pos2 pos) const;

	inline int idx(Pos pos) const {
		assert(in_bounds(pos));
		return pos.x + pos.y * dimensions.x + pos.z * dimensions.y * dimensions.x;
	}

	inline uint16_t& substance(int idx) { return substance_grid[idx]; }
	inline uint16_t& substance(Pos pos) { return substance(idx(pos)); }

	inline Form& form(int idx) { return form_grid[idx]; }
	inline Form& form(Pos pos) { return form(idx(pos)); }

	inline bool& visible(int idx) { return visibility_grid[idx]; }
	inline bool& visible(Pos pos) { return visible(idx(pos)); }

	inline uint16_t& machine(int idx) { return machine_grid[idx]; }
	inline uint16_t& machine(Pos pos) { return machine(idx(pos)); }

	Cell get_cell(Pos pos);
	const Cell get_cell(Pos pos) const;
	bool is_walkable(Pos pos) const;

private:
	Pos dimensions;
	std::unique_ptr<uint16_t[]> substance_grid;
	std::unique_ptr<Form[]> form_grid;
	std::unique_ptr<bool[]> visibility_grid;
	std::unique_ptr<uint16_t[]> machine_grid;
};


#endif //UNTITLED_REGION_H
