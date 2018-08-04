#include "Path.h"
#include <queue>
#include <cassert>
#include <limits>
#include <cstring>
#include <algorithm>

struct CompareCell : public std::binary_function<std::pair<Pos, PathFinder::Node*>, std::pair<Pos, PathFinder::Node*>, bool> {
	bool operator()(const std::pair<Pos, PathFinder::Node*>& lhs, const std::pair<Pos, PathFinder::Node*>& rhs) const {
		return (int)lhs.second->g + lhs.second->h > (int)rhs.second->g + rhs.second->h;
	}
};

PathFinder::PathFinder(Region& region): region(region) {
	nodes.reset(new Node[region.dim().x * region.dim().y * region.dim().z]);
}

int get_g(Pos dir) {
	return 3 + (dir.x && dir.y) + ((bool)dir.z) * 2;
}

unsigned short get_h(Pos diff) {
	diff = diff.abs();
	int h = diff.z * 2;
	if (diff.x > diff.y) {
		h += diff.y * 4 + (diff.x - diff.y) * 3;
	} else {
		h += diff.x * 4 + (diff.y - diff.x) * 3;
	}
	return (unsigned short)std::min(h, (int)std::numeric_limits<unsigned short>::max());
}

bool PathFinder::go(Pos from, Dir dir, Pos& dest) {
	dest = Pos::from_dir(dir) + from;
	if (region.form(dest) == Form::Ramp && region.is_walkable(dest + Pos(0, 0, 1))) {
		dest += Pos(0, 0, 1);
	} else if (!region.is_walkable(dest)) {
		if (region.get_cell(dest - Pos(0, 0, 1)).form == Form::Ramp) {
			dest -= Pos(0, 0, 1);
		} else {
			return false;
		}
	}
	return true;
}

std::vector<Pos> PathFinder::create_path(Pos start, Pos goal) {
	unsigned short OPEN = cur_id;
	unsigned short DONE = (unsigned short)(cur_id + 1);

	std::priority_queue<std::pair<Pos, Node*>, std::vector<std::pair<Pos, Node*>>, CompareCell> open;

	Node& start_node = at(start);
	start_node.path_status = OPEN;
	start_node.prev = NONE;
	start_node.g = 0;
	start_node.h = get_h(goal - start);
	open.push(std::make_pair(start, &start_node));

	bool success = false;
	int iterations = 0;
	while (true) {
		if (open.empty() || iterations > 10000) {
			break;
		}

		Pos current = open.top().first;
		Node& current_node = *open.top().second;
		open.pop();
		current_node.path_status = DONE;
		iterations++;

		Pos gdiff = (goal - current).abs();
		if (gdiff.x <= 1 && gdiff.y <= 1 && gdiff .z <= 1) {
			Node& goal_node = at(goal);
			goal_node.prev = pos_to_dir(gdiff);
			goal_node.prev_z = (int8_t)(goal.z - current.z);
			success = true;
			break;
		}

		for (int i = 1; i < Dir::Count; i++) {
			Pos dest;
			if (!go(current, (Dir)i, dest)) continue;

			Node& dest_node = at(dest);
			if (dest_node.path_status == DONE) {
				continue;
			}

			if (dest_node.path_status < OPEN) {
				Pos diff = dest - current;
				int g = get_g(diff);
				if (g > std::numeric_limits<unsigned short>::max()) {
					dest_node.path_status = DONE;
					continue;
				}

				dest_node.path_status = OPEN;
				dest_node.prev = (Dir)i;
				dest_node.prev_z = (int8_t)diff.z;
				dest_node.g = (unsigned short)g;

				dest_node.h = get_h(goal - dest);
				open.push(std::make_pair(dest, &dest_node));
			}
		}
	}

	std::vector<Pos> path;
	if (success) {
		Pos cur = goal;
		while (true) {
			path.push_back(cur);
			if (cur == start) {
				break;
			}
			Node& node = at(cur);
			cur = cur - Pos::from_dir(node.prev) - Pos(0, 0, node.prev_z);
		}
		std::reverse(path.begin(), path.end());
	}

	if ((int)cur_id + 5 >= (int)std::numeric_limits<unsigned short>::max()) {
		int len = region.dim().x * region.dim().y * region.dim().z;
		memset(nodes.get(), 0, sizeof(Cell) * len);
		cur_id = 1;
	}
	cur_id += 2;

	return path;
}

PathFinder::Node& PathFinder::at(Pos pos) {
	assert(region.in_bounds(pos));
	return nodes[pos.x + pos.y * region.dim().x + pos.z * region.dim().y * region.dim().x];
}
