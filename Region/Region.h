#ifndef UNTITLED_REGION_H
#define UNTITLED_REGION_H

#include <vector>
#include <memory>
#include "Pos.h"

enum class Form: uint8_t {
	None,
	Fill,
	Ramp,
	Wall,
	Floor
};


struct Cell {
	uint16_t substance = 0;
	Form form = Form::None;
	bool visible = false;
};

class Region {
public:
	explicit Region(Pos dimensions);

	Pos dim() const;
	bool in_bounds(Pos pos) const;
	bool in_bounds(Pos2 pos) const;

	Cell& get_cell(Pos pos);
	const Cell& get_cell(Pos pos) const;
	bool is_walkable(Pos pos) const;

private:
	Pos dimensions;
	std::unique_ptr<Cell[]> cells;
};


#endif //UNTITLED_REGION_H
