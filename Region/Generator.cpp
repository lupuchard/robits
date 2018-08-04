#include <iostream>
#include "Generator.h"
#include "../Deps/FastNoise.h"

void Generator::generate(Region& region) {
	FastNoise height_noise1((int)time(nullptr));
	height_noise1.SetNoiseType(FastNoise::Perlin);
	height_noise1.SetFrequency(0.1);
	height_noise1.SetInterp(FastNoise::Hermite);

	FastNoise height_noise2((int)time(nullptr));
	height_noise1.SetNoiseType(FastNoise::Perlin);
	height_noise1.SetFrequency(0.02);
	height_noise1.SetInterp(FastNoise::Hermite);

	std::vector<int> heights;
	heights.resize((size_t)(region.dim().x * region.dim().y));
	int max_range = std::min(10, region.dim().z / 2);

	std::cout << "Generating terrain..." << std::endl;
	for (int16_t y = 0; y < region.dim().y; y++) {
		for (int16_t x = 0; x < region.dim().x; x++) {
			float noise_value = height_noise1.GetValue(x, y) / 2 + height_noise2.GetValue(x, y) / 2;
			noise_value = (noise_value * noise_value * noise_value);
			heights[x + y * region.dim().x] = (int)((noise_value + 1.f) / 2 * max_range) + region.dim().z / 2 - max_range / 2;
		}
	}

	std::cout << "Filling cells..." << std::endl;
	for (int16_t z = 0; z < region.dim().z; z++) {
		for (int16_t y = 0; y < region.dim().y; y++) {
			for (int16_t x = 0; x < region.dim().x; x++) {
				if (z <= heights[x + y * region.dim().x] || x == 0 || y == 0 || z == 0 ||
				    x == region.dim().x - 1 || y == region.dim().y - 1 || z == region.dim().z - 1) {

					int idx = region.idx(Pos(x, y, z));
					region.form(idx) = Form::Fill;
					region.substance(idx) = 1;
				}
			}
		}
	}

	FastNoise ramp_noise((int)time(nullptr));
	ramp_noise.SetNoiseType(FastNoise::Perlin);
	ramp_noise.SetFrequency(0.05);
	ramp_noise.SetInterp(FastNoise::Linear);

	std::cout << "Determining ramps..." << std::endl;
	for (int z = 1; z < region.dim().z - 1; z++) {
		for (int y = 1; y < region.dim().y - 1; y++) {
			for (int x = 1; x < region.dim().x - 1; x++) {
				Form& form = region.form(Pos(x, y, z));

				bool rampable = form == Form::Fill && region.form(Pos(x, y, z + 1)) != Form::Fill &&
				                ramp_noise.GetValue(x, y, z) < 0;
				if (!rampable) continue;

				for (int x0 = -1; x0 <= 1; x0++) {
					for (int y0 = -1; y0 <= 1; y0++) {
						if (!(x0 == 0 || y0 == 0)) continue;
						if (x0 == 0 && y0 == 0) continue;
						int x1 = x0 + x;
						int y1 = y0 + y;

						if (region.form(Pos(x1, y1, z)) == Form::None &&
						    region.form(Pos(x1, y1, z - 1)) != Form::None) {

							form = Form::Ramp;
							goto exit_doubleloop;
						}
					}
				}
exit_doubleloop:;
			}
		}
	}

	std::cout << "Determining visibility..." << std::endl;
	for (int z = 1; z < region.dim().z - 1; z++) {
		for (int y = 1; y < region.dim().y - 1; y++) {
			for (int x = 1; x < region.dim().x - 1; x++) {
				bool& visible = region.visible(Pos(x, y, z));

				for (int x0 = -1; x0 <= 1; x0++) {
					for (int y0 = -1; y0 <= 1; y0++) {
						if (x0 == 0 && y0 == 0) continue;
						if (region.get_cell(Pos(x0 + x, y0 + y, z)).form == Form::None) {
							visible = true;
							goto exit_doubleloop2;
						}
					}
				}
exit_doubleloop2:;
			}
		}
	}
}
