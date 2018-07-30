#ifndef TOWN_UNIT_H
#define TOWN_UNIT_H

#include "Region/Pos.h"
#include "JobManager.h"
#include <vector>

class Unit {
public:
	Unit(int id): _id(id) { }

	int id() const { return _id; }

	enum State: uint16_t {
		IDLE,
		PENDING,
		TRAVEL,
	};

	inline void assign_job(Job& job) {
		current_job = &job;
		state = PENDING;
	}

	short overticks = 0;
	State state = IDLE;
	Job* current_job = nullptr;

	int path_pos = 0;
	std::vector<Pos> path;
	Pos pos;

private:
	int _id;
};


#endif //TOWN_UNIT_H
