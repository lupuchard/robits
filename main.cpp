#include <iostream>
#include <vector>
#include <cassert>
#include "Region/Generator.h"
#include "Renderer.h"
#include <SFML/Graphics.hpp>

/*#define NUM_ACTIONS 100
#define NUM_IMPS 1000
#define NUM_SPLITTIES 10
#define NUM_IMP_PER_SPLITTY NUM_IMPS / NUM_SPLITTIES*/

int16_t scrollX = 0;
int16_t scrollY = 0;

int main() {
	Game game;
	Menu& menu = game.get_menu();
	sf::RenderWindow window(sf::VideoMode(1520, 800), "Hey");
	window.setFramerateLimit(60);
	Renderer renderer(window, game);

	while (window.isOpen()) {
		if (!game.is_paused()) {
			game.tick(2);
		}

		sf::Vector2f mouse_pos;
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
				case sf::Event::Closed:
					window.close();
					break;
				case sf::Event::KeyPressed:
					if (!event.key.shift) {
						switch (event.key.code) {
							case sf::Keyboard::Left:  case sf::Keyboard::A:
								scrollX = -1;
								break;
							case sf::Keyboard::Right: case sf::Keyboard::E:
								scrollX = 1;
								break;
							case sf::Keyboard::Up:    case sf::Keyboard::Comma:
								scrollY = -1;
								break;
							case sf::Keyboard::Down:  case sf::Keyboard::O:
								scrollY = 1;
								break;
							default: break;
						}
					}
					break;
				case sf::Event::KeyReleased:
					if (!event.key.shift) {
						switch (event.key.code) {
							case sf::Keyboard::Left:  case sf::Keyboard::A:
							case sf::Keyboard::Right: case sf::Keyboard::E:
								scrollX = 0;
								break;
							case sf::Keyboard::Up:    case sf::Keyboard::Comma:
							case sf::Keyboard::Down:  case sf::Keyboard::O:
								scrollY = 0;
								break;
							case sf::Keyboard::Space:
								if (game.is_paused()) game.resume(); else game.pause();
								renderer.update_menu();
							default: break;
						}
					}
					break;
				case sf::Event::TextEntered:
					switch (event.text.unicode) {
						case '<':
							renderer.shift(1);
							break;
						case '>':
							renderer.shift(-1);
							break;
						case '\'':
							menu.back_out();
							break;
						default: break;
					}
					menu.trigger_select(menu.current(), (char)event.text.unicode);
					renderer.update_menu();
					break;
				case sf::Event::MouseWheelScrolled:
					if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
						event.mouseWheelScroll.delta > 0 ? renderer.zoom_in() : renderer.zoom_out();
					}
					break;
				case sf::Event::MouseButtonPressed:
					renderer.click();
					break;
				default: break;
			}
		}

		window.clear();

		if (scrollX != 0 || scrollY != 0) {
			renderer.move(Pos2(scrollX, scrollY));
		}
		renderer.render();

		window.display();
	}

	return 0;
}

/*template<typename T>
int partition(std::vector<T>& list, int p, int r) {
	T pivot = list[r];
	while (p < r) {
		while (list[p] > pivot) p++;
		while (list[r] < pivot) r--;
		if (list[p] == list[r]) {
			p++;
		} else if (p < r) {
			T tmp = list[p];
			list[p] = list[r];
			list[r] = tmp;
		}
	}
	return r;
}
template<typename T>
T quick_select(std::vector<T>& list, int p, int r, int k) {
	if (p == r) return list[p];
	int j = partition(list, p, r);
	int length = j - p + 1;
	if (length == k) return list[j];
	else if (k < length) return quick_select(list, p, j - 1, k);
	else return quick_select(list, j + 1, r, k - length);
}
template<typename T>
T quick_select(std::vector<T> list, int k) {
	assert(!list.empty());
	return quick_select<T>(list, 0, (int)list.size() - 1, k);
}


struct Floot {
	Floot() {
		v1 = (float)rand() / RAND_MAX;
		v2 = (float)rand() / RAND_MAX;
		v3 = (float)rand() / RAND_MAX;
		v4 = (float)rand() / RAND_MAX;
	}
	float v1, v2, v3, v4;
};

struct Action {
	std::vector<Floot> weights;
};

int main() {
	srand(time(nullptr));

	std::vector<Action> actions;
	for (int i = 0; i < NUM_ACTIONS; i++) {
		actions.emplace_back();
		for (int j = 0; j < NUM_IMPS; j++) {
			actions[i].weights.emplace_back();
		}
	}

	std::vector<Floot> imp_results;
	for (int i = 0; i < NUM_IMPS; i++) {
		imp_results.emplace_back();
	}

	float max = 0;
	int correct_result = -1;
	for (int i = 0; i < NUM_ACTIONS; i++) {
		float total = 0;
		for (int j = 0; j < NUM_IMPS; j++) {
			total += actions[i].weights[j].v1 * imp_results[j].v1;
			total += actions[i].weights[j].v2 * imp_results[j].v2;
			total += actions[i].weights[j].v3 * imp_results[j].v3;
			total += actions[i].weights[j].v4 * imp_results[j].v4;
		}
		if (total > max) {
			max = total;
			correct_result = i;
		}
	}

	std::cout << "Correct result: " << correct_result << std::endl;

	std::vector<int> remaining_actions;
	remaining_actions.reserve(NUM_ACTIONS);
	for (int i = 0; i < NUM_ACTIONS; i++) {
		remaining_actions.push_back(i);
	}
	for (int i = 0; i < NUM_SPLITTIES; i++) {
		if (remaining_actions.size() <= 1) break;

		std::vector<float> totals;
		totals.reserve(remaining_actions.size());
		for (int j = 0; j < remaining_actions.size(); j++) {
			float total = 0;
			for (int k = i * NUM_IMP_PER_SPLITTY; k < (i + 1) * NUM_IMP_PER_SPLITTY; k++) {
				total += actions[j].weights[k].v1 * imp_results[k].v1;
				total += actions[j].weights[k].v2 * imp_results[k].v2;
				total += actions[j].weights[k].v3 * imp_results[k].v3;
				total += actions[j].weights[k].v4 * imp_results[k].v4;
			}
			totals.push_back(total);
		}
		float nth_largest = quick_select(totals, (int)(totals.size() / 2));

		int j = 0;
		for (int k = 0; k < totals.size(); k++) {
			if (totals[k] >= nth_largest) {
				remaining_actions[j] = remaining_actions[k];
				j++;
				if (remaining_actions[j] == correct_result) {
					std::cout << "Made it to round " << i << std::endl;
				}
			}
		}
		remaining_actions.resize(totals.size() / 2);
	}
	std::cout << "Approx result: " << remaining_actions[0] << std::endl;

	return 0;
}*/