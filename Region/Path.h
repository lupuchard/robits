#ifndef TOWN_PATH_H
#define TOWN_PATH_H

#include "Region.h"

class PathFinder {
public:
	struct Node {
		unsigned short g; // distance to start
		unsigned short h; // estimated distance to end
		unsigned short path_status = 0;
		Dir prev;
		int8_t prev_z;
	};

	PathFinder(Region& region);
	std::vector<Pos> create_path(Pos start, Pos end);

private:
	bool go(Pos from, Dir dir, Pos& dest);
	Node& at(Pos pos);

	Region& region;
	unsigned short cur_id = 1;
	std::unique_ptr<Node[]> nodes;
};


#endif //TOWN_PATH_H
