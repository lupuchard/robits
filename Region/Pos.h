#ifndef UNTITLED_POS_H
#define UNTITLED_POS_H

#include <cstdint>
#include <cmath>

enum Dir: uint8_t {
	NONE,
	NORTH,
	NORTH_EAST,
	EAST,
	SOUTH_EAST,
	SOUTH,
	SOUTH_WEST,
	WEST,
	NORTH_WEST,
	Count,
};

struct Pos {
	Pos(): x(0), y(0), z(0) { }
	Pos(int16_t x, int16_t y, int16_t z): x(x), y(y), z(z) { }
	Pos(int x, int y, int z): x((int16_t)x), y((int16_t)y), z((int16_t)z) { }

	static Pos from_dir(Dir dir) {
		switch (dir) {
			case NORTH:      return Pos( 0,  1, 0);
			case NORTH_EAST: return Pos( 1,  1, 0);
			case EAST:       return Pos( 1,  0, 0);
			case SOUTH_EAST: return Pos( 1, -1, 0);
			case SOUTH:      return Pos( 0, -1, 0);
			case SOUTH_WEST: return Pos(-1, -1, 0);
			case WEST:       return Pos(-1,  0, 0);
			case NORTH_WEST: return Pos(-1,  1, 0);
			default:         return Pos( 0,  0, 0);
		}
	}

	inline Pos abs() {
		return Pos((int16_t)std::abs(x), (int16_t)std::abs(y), (int16_t)std::abs(z));
	}

	inline Pos& operator+=(const Pos& rhs) {
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}
	inline Pos& operator-=(const Pos& rhs) {
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}

	int16_t x, y, z;
};

inline Dir pos_to_dir(Pos pos) {
	if (pos.x < 0) {
		return pos.y < 0 ? SOUTH_WEST : pos.y == 0 ? WEST : NORTH_WEST;
	} else if (pos.x == 0) {
		return pos.y < 0 ? SOUTH      : pos.y == 0 ? NONE : NORTH;
	} else {
		return pos.y < 0 ? SOUTH_EAST : pos.y == 0 ? EAST : NORTH_EAST;
	}
}

inline Pos operator+(Pos lhs, const Pos& rhs) {
	lhs += rhs;
	return lhs;
}
inline Pos operator-(Pos lhs, const Pos& rhs) {
	lhs -= rhs;
	return lhs;
}
inline bool operator==(const Pos& lhs, const Pos& rhs) {
	return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

struct Pos2 {
	Pos2(): x(0), y(0) { }
	Pos2(int16_t x, int16_t y): x(x), y(y) { }
	Pos2(int x, int y): x((int16_t)x), y((int16_t)y) { }

	inline Pos2 operator*=(int rhs) {
		x *= rhs;
		y *= rhs;
		return *this;
	}

	int16_t x, y;
};

inline Pos2 operator*(Pos2 lhs, int rhs) {
	lhs *= rhs;
	return lhs;
}


#endif //UNTITLED_POS_H
