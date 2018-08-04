#include "Substance.h"
#include <cstddef>
#include <cmath>

const float PARTICULATE_SORTER_EFFICIENCY = 0.4f; // the lower the better
const float MINIMUM_AMOUNT = 0.0001f; // 0.01%

// in kg/m^3
// values retrieved from the wolfrem elpha
float get_density(Substance substance) {
	switch (substance) {
		case Nothing:        return 0;

		case PigIron:        return 7000;
		case Steel:          return 7800;

		case Silica:         return 2633;
		case Forsterite:     return 3270;
		case Fayalite:       return 4390;
		case Pyroxene:       return 3225;
		case Orthoclase:     return 2560;
		case Albite:         return 2620;
		case Anorthite:      return 2735;
		case Melilite:       return 2950;
		case Serpentine:     return 2450;
		case Amphibole:      return 3200;
		case Mica:           return 2150;

		case Calcite:        return 2710;
		case Dolomite:       return 2850;
		case Magnesite:      return 3000;
		case Siderite:       return 3960;

		case Alunite:        return 2740;
		case Anhydrite:      return 2970;

		case Magnesia:       return 1740;
		case Quicklime:      return 3350;
		case Rutile:         return 4230;
		case Hematite:       return 5260;
		case Magnetite:      return 5150;
		case Ilmenite:       return 4550;
		case Chromite:       return 4790;

		case Gibbsite:       return 2340;

		case Iron:           return 7874;

		case Graphite:       return 2266;

		case Oxygen:         return 1;
		case CarbonDioxide:  return 1;
	}
	return 0;
}

// celsius
// values retrieved from the doggysheep beta
float get_melting_point(Substance substance) {
	switch (substance) {
		case Nothing:        return 0;

		case PigIron:        return 1200;
		case Steel:          return 1500;

		case Silica:         return 1400;
		case Forsterite:     return 1900;
		case Fayalite:       return 1200;
		case Pyroxene:       return 1400;
		case Orthoclase:     return 1300;
		case Albite:         return 1100;
		case Anorthite:      return 1500;
		case Melilite:       return 1500;
		case Serpentine:     return  800;
		case Amphibole:      return 1100;
		case Mica:           return 1300;

		case Calcite:        return 9999;
		case Dolomite:       return 9999;
		case Magnesite:      return 9999;
		case Siderite:       return 9999;

		case Alunite:        return 2000;
		case Anhydrite:      return 1400;

		case Magnesia:       return 700;
		case Quicklime:      return 2600;
		case Rutile:         return 1800;
		case Hematite:       return 1600;
		case Magnetite:      return 1600;
		case Ilmenite:       return 1000;
		case Chromite:       return 2200;

		case Gibbsite:       return 300;

		case Iron:           return 1500;

		case Graphite:       return 4000;

		case Oxygen:         return -220;
		case CarbonDioxide:  return  -78;
	}
	return 0;
}

float get_boiling_point(Substance substance) {
	switch (substance) {
		case Nothing:        return 0;

		case PigIron:        return 2200;
		case Steel:          return 2800;

		case Silica:         return 2200;
		case Forsterite:     return 2700;
		case Fayalite:       return 2000;
		case Pyroxene:       return 2200;
		case Orthoclase:     return 2100;
		case Albite:         return 1900;
		case Anorthite:      return 2300;
		case Melilite:       return 2300;
		case Serpentine:     return 1600;
		case Amphibole:      return 1900;
		case Mica:           return 2100;

		case Calcite:        return 9999;
		case Dolomite:       return 9999;
		case Magnesite:      return 9999;
		case Siderite:       return 9999;

		case Alunite:        return 3000;
		case Anhydrite:      return 3000;

		case Magnesia:       return 3600;
		case Quicklime:      return 2800;
		case Rutile:         return 3000;
		case Hematite:       return 2900;
		case Magnetite:      return 2900;
		case Ilmenite:       return 2400;
		case Chromite:       return 3200;

		case Gibbsite:       return 2800;

		case Iron:           return 2800;

		case Graphite:       return 9999;

		case Oxygen:         return -180;
		case CarbonDioxide:  return  -78;
	}
	return 0;
}

float get_magnetic_strength(Substance substance) {
	switch (substance) {
		case Siderite:  return 0.1;
		case Hematite:  return 0.1;
		case Magnetite: return 0.9;
		case Chromite:  return 0.3;
		case Ilmenite:  return 0.5;
		default: return 0;
	}
}

Decomposition* get_decomposition(Substance substance) {
	switch (substance) {
		case Calcite: {
			static Decomposition decomposition(800,
				{ Quicklime, CarbonDioxide },
				{ 0.56, 0.44 }
			);
			return &decomposition;
		}
		case Dolomite: {
			static Decomposition decomposition(700,
				{ Calcite, Magnesia, CarbonDioxide },
				{ 0.54, 0.22, 0.24 }
			);
			return &decomposition;
		}
		case Magnesite: {
			static Decomposition decomposition(900,
				{ Magnesia, CarbonDioxide },
				{ 0.48, 0.52 }
			);
			return &decomposition;
		}
		case Siderite: {
			static Decomposition decomposition(500,
				{ Magnetite, Graphite, Oxygen },
				{ 0.67, 0.10, 0.23 }
			);
			return &decomposition;
		}
		default: return nullptr;
	}
}

void Mixture::add_substance(Substance substance, float amount) {
	substances.push_back(substance);
	amounts.push_back(amount);
}
void Mixture::add_substance_stack(Substance substance, float amount) {
	for (size_t i = 0; i < substances.size(); i++) {
		if (substances[i] == substance) {
			amounts[i] += amount;
			return;
		}
	}
	add_substance(substance, amount);
}

float Mixture::normalize() {
	float total = 0;
	for (float amount : amounts) {
		total += amount;
	}
	for (size_t i = 0; i < amounts.size(); i++) {
		amounts[i] /= total;
		if (amounts[i] < MINIMUM_AMOUNT) {
			amounts.erase(amounts.begin() + i);
			substances.erase(substances.begin() + i);
			i--;
		}
	}
	return total;
}

Mixture Mixture::mix(Mixture& other, float amount) {
	Mixture output = *this;
	output.processes_completed |= other.processes_completed;
	for (size_t i = 0; i < other.substances.size(); i++) {
		output.add_substance_stack(other.substances[i], other.amounts[i] * amount);
	}
	output.normalize();
	return output;
}

Mixture::ProcessingResult Mixture::release_gases(float temperature) const {
	ProcessingResult result(state, GAS, processes_completed);

	for (size_t i = 0; i < substances.size(); i++) {
		if (get_boiling_point(substances[i]) < temperature) {
			result.byproduct->add_substance(substances[i], amounts[i]);
		} else {
			result.product->add_substance(substances[i], amounts[i]);
		}
	}

	result.normalize();
	return result;
}

Mixture::ProcessingResult Mixture::sort_particulate(Substance target) const {
	if (state != PARTICULATE || (processes_completed & SORT_PARTICULATE)) {
		return ProcessingResult();
	}

	ProcessingResult result(PARTICULATE, PARTICULATE, processes_completed);

	float target_density = get_density(target);
	for (size_t i = 0; i < substances.size(); i++) {
		if (substances[i] == target) {
			result.product->add_substance(target, amounts[i]);
		} else {
			float density_ratio = target_density / get_density(substances[i]);
			if (density_ratio < 1) density_ratio = 1 / density_ratio;
			float amount_removed = 1 - std::pow(PARTICULATE_SORTER_EFFICIENCY, density_ratio - 0.9f);
			result.byproduct->add_substance(substances[i], amount_removed * amounts[i]);
			result.product->add_substance(substances[i], (1 - amount_removed) * amounts[i]);
		}
	}

	result.normalize();
	result.product->processes_completed |= SORT_PARTICULATE;

	return result;
}

Mixture::ProcessingResult Mixture::sort_magnetic() const {
	if (state != PARTICULATE || (processes_completed & SORT_MAGNETIC)) {
		return ProcessingResult();
	}

	ProcessingResult result(PARTICULATE, PARTICULATE, processes_completed);

	for (size_t i = 0; i < substances.size(); i++) {
		float mag = get_magnetic_strength(substances[i]);
		result.product->add_substance(substances[i], mag * amounts[i]);
		result.byproduct->add_substance(substances[i], (1 - mag) * amounts[i]);
	}

	result.normalize();
	result.product->processes_completed |= SORT_MAGNETIC;

	return result;
}

Mixture::SmeltingResult Mixture::smelt_pig_iron(float& graphite_needed) const {
	if (state != PARTICULATE) {
		return SmeltingResult();
	}

	SmeltingResult result(PigIron, MOLTEN, STONE, processes_completed);

	float total_iron = 0;
	float total_oxygen = 0;
	float total_carbon = 0;
	float total_impurities = 0;

	std::vector<Substance> substances = this->substances;
	std::vector<float> amounts = this->amounts;

	for (size_t i = 0; i < substances.size(); i++) {
		if (substances[i] == Hematite) {
			total_iron += amounts[i] * 0.6994;
			total_oxygen += amounts[i] * 0.3006;
		} else if (substances[i] == Magnetite) {
			total_iron += amounts[i] * 0.7236;
			total_oxygen += amounts[i] * 0.2764;
		} else if (substances[i] == Iron) {
			total_iron += amounts[i];
		} else if (substances[i] == Graphite) {
			total_carbon += amounts[i];
		} else {
			Decomposition* decomposition = get_decomposition(substances[i]);
			if (decomposition != nullptr && decomposition->temperature <= 1300) {
				for (size_t j = 0; j < decomposition->results.size(); j++) {
					substances.push_back(decomposition->results[j]);
					amounts.push_back(decomposition->quantities[j] * amounts[i]);
				}
			} else {
				float melting_point = get_melting_point(substances[i]);
				if (melting_point <= 1300) {
					result.byproduct->add_substance(substances[i], amounts[i]);
				} else if (melting_point <= 1400) {
					result.byproduct->add_substance(substances[i], amounts[i] / 2);
					total_impurities += amounts[i] / 2;
				} else {
					total_impurities += amounts[i];
				}
			}
		}
	}

	// pig iron is 4% carbon
	total_carbon -= total_iron * 0.04;

	// carbon makes up 72.71% of CO2
	total_carbon -= total_oxygen * (27.29 / 72.71);
	result.byproduct->add_substance(CarbonDioxide, total_oxygen / 0.7271f);

	if (total_carbon > 0) {
		result.byproduct->add_substance(Graphite, total_carbon);
		graphite_needed = 0;
	} else {
		graphite_needed = -total_carbon;
	}

	result.product.grade = get_grade(total_impurities / (total_impurities + total_iron + 0.02f));

	return result;
}

uint8_t Mixture::get_grade(float impurity) {
	if (impurity < 0.0001) {
		return 7; // S+
	} else if (impurity < 0.0003) {
		return 6; // S
	} else if (impurity < 0.001) {
		return 5; // A
	} else if (impurity < 0.003) {
		return 4; // B
	} else if (impurity < 0.01) {
		return 3; // C
	} else if (impurity < 0.03) {
		return 2; // D
	} else if (impurity < 0.1) {
		return 1; // E
	} else {
		return 0; // F (failed)
	}
}

