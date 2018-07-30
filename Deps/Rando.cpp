#include "Rando.h"
#include <limits>

Rando::Rando(uint64_t seed) {
	for (int i = 0; i < 4; i++) {
		uint64_t z = (seed += 0x9e3779b97f4a7c15);
		z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
		z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
		s[i] = z ^ (z >> 31);
	}
}

uint64_t rotl(uint64_t x, int k) {
	return (x << k) | (x >> (64 - k));
}

uint64_t Rando::rand() {
	const uint64_t result_starstar = rotl(s[1] * 5, 7) * 9;

	const uint64_t t = s[1] << 17;

	s[2] ^= s[0];
	s[3] ^= s[1];
	s[1] ^= s[2];
	s[0] ^= s[3];

	s[2] ^= t;

	s[3] = rotl(s[3], 45);

	return result_starstar;
}

int64_t Rando::rand(int64_t min, int64_t max) {
	return rand() % (max - min) + min;
}

double Rando::rand_float() {
	return (double)rand() / max();
}

uint64_t Rando::min() {
	return 0;
}

uint64_t Rando::max() {
	return std::numeric_limits<uint64_t>::max();
}
