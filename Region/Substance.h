#ifndef TOWN_SUBSTANCE_H
#define TOWN_SUBSTANCE_H

#include <vector>
#include <cstdint>
#include <memory>

enum Substance {
	Nothing,

	// Materials
	PigIron,
	Steel,

	// Silicates
	Silica,
	Forsterite,
	Fayalite,  // Olivine
	Pyroxene,  // ''
	Orthoclase,// Feldspar
	Albite,    // ''
	Anorthite, // ''
	Melilite,
	Serpentine,
	Amphibole,
	Mica,

	// Carbonates
	Calcite,
	Dolomite,
	Magnesite,
	Siderite,

	// Sulfates
	Alunite,
	Anhydrite,

	// Oxides,
	Magnesia,  // magnesium
	Quicklime, // calcium
	Rutile,    // titanium
	Hematite,  // iron
	Magnetite, // iron
	Ilmenite,  // iron + titanium
	Chromite,  // iron + chromium

	// Hydroxide
	Gibbsite, // aluminum

	// Pure Metals,
	Iron,

	// Gases
	Oxygen,
	CarbonDioxide,

	// Misc
	Graphite,
};

float get_density(Substance substance); // in kg/m^3
float get_magnetic_strength(Substance substance);
float get_melting_point(Substance substance);
float get_boiling_point(Substance substance);

struct Decomposition {
	Decomposition(float temperature, std::vector<Substance> results, std::vector<float> quantities):
		temperature(temperature), results(results), quantities(quantities) { }
	float temperature;
	std::vector<Substance> results;
	std::vector<float> quantities;
};
Decomposition* get_decomposition(Substance substance);


struct Material {
	uint16_t substance;
	uint8_t grade;
	uint8_t state;
};


class Mixture {
public:

	enum State: uint32_t {
		STONE,
		PARTICULATE,
		MOLTEN,
		INGOT,
		GAS,
	};

	enum Process: uint32_t {
		NONE = 0,
		SORT_PARTICULATE = 1,
		SORT_MAGNETIC = 2,
	};

	struct ProcessingResult {
		ProcessingResult() = default;
		ProcessingResult(State product_state, State byproduct_state, uint32_t processes_completed) {
			success = true;
			product.reset(new Mixture(product_state, processes_completed));
			byproduct.reset(new Mixture(byproduct_state, processes_completed));
		}
		inline void normalize() {
			float product_total = product->normalize();
			float byproduct_total = byproduct->normalize();
			ratio = product_total / byproduct_total;
		}
		bool success = false;
		float ratio = 0;
		std::unique_ptr<Mixture> product;
		std::unique_ptr<Mixture> byproduct;
	};

	struct SmeltingResult {
		SmeltingResult() = default;
		SmeltingResult(Substance product_substance, State product_state, State byproduct_state,
				uint32_t processes_completed) {
			success = true;
			product.substance = product_substance;
			product.grade = 0;
			product.state = product_state;
			byproduct.reset(new Mixture(byproduct_state, processes_completed));
		}

		bool success = false;
		Material product;
		float ratio = 0;
		std::unique_ptr<Mixture> byproduct;
	};

	Mixture(State state, uint32_t processes_completed = 0):
		state(state), processes_completed(processes_completed) { }

	void add_substance(Substance substance, float amount);
	void add_substance_stack(Substance substance, float amount);
	float normalize();

	Mixture mix(Mixture& other, float amount);
	ProcessingResult release_gases(float temperature) const;
	ProcessingResult sort_particulate(Substance target) const;
	ProcessingResult sort_magnetic() const;

	SmeltingResult smelt_pig_iron(float& graphite_needed) const;

private:
	static uint8_t get_grade(float impurity);

	std::vector<Substance> substances;
	std::vector<float> amounts;
	State state = STONE;
	uint32_t processes_completed = 0;
};

// 1. Crusher
//   Ore to crushed mixture
// 2. Particulate Sorter
//   Combination of sizers and density separators. A mixture can only be sorted once,
//   targeting density but also a bonus to represent sizing
// 3. Leacher
//   Requires a liquid. Separates particles by their ability to dissolve in the given liquid.
//   Research will have to be done on common liquids used in leaching.
// 4. Magnetic Sorter
//   Separates magnetic particulate, such as magnetite.
// 5. Smelter
//   Sorts by melting point of particulate.

// 6. Steelworks
//   Takes in melted iron oxide

// Basic iron processing could probably just be:
//   Crusher -> Particulate Sorter -> Magnetic Sorter -> Smelter

//

// density
// dissolves (leaching)
// melting point
// boiling point
// magnetic
// hydrophobia (froth flotation)


#endif //TOWN_SUBSTANCE_H
