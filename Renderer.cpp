#include "Renderer.h"
#include "Deps/Rando.h"

const int CELL_SIZE = 16;
const float DEPTH_FACTOR = 0.7f;
const float SQRT_DEPTH_FACTOR = std::sqrt(0.7f);
const int MENU_PANE_WIDTH = 300;

Renderer::Renderer(sf::RenderWindow& window, Game& game): window(window), game(game) {
	font.loadFromFile("Resources/courier.ttf");
	tiles.loadFromFile("Resources/tiles.png");

	game.set_renderer(*this);
	Region& region = game.get_region();

	z_level = region.dim().z / 2;
	x_offset = 1;
	y_offset = 1;
	screen_w = window.getSize().x - MENU_PANE_WIDTH;
	screen_h = window.getSize().y;

	ground_heights.reset(new int16_t[region.dim().x * region.dim().y]);

	ground_texture.create((unsigned int)(region.dim().x * CELL_SIZE), (unsigned int)(region.dim().y * CELL_SIZE));
	menu_texture.create(MENU_PANE_WIDTH, (unsigned int)screen_h);

	update_ground();
	update_menu();
}

sf::Color darken(sf::Color color, int amount, float factor = DEPTH_FACTOR) {
	float darkening = std::pow(factor, (float)amount);
	color.r = (uint8_t)(darkening * color.r);
	color.g = (uint8_t)(darkening * color.g);
	color.b = (uint8_t)(darkening * color.b);
	return color;
}

void Renderer::render() {
	sf::Sprite ground_sprite(ground_texture.getTexture());
	ground_sprite.setPosition(-x_offset * CELL_SIZE * zoom, -y_offset * CELL_SIZE * zoom);
	ground_sprite.setScale(zoom, zoom);
	window.draw(ground_sprite);

	sf::Sprite menu_sprite(menu_texture.getTexture());
	menu_sprite.setPosition(screen_w, 0);
	window.draw(menu_sprite);


	Region& region = game.get_region();
	UnitManager& unit_manager = game.get_unit_manager();

	tex_vertices.clear();

	Pos2 min((int16_t)x_offset, (int16_t)y_offset);
	Pos2 max((int16_t)(x_offset + (float)screen_w / (CELL_SIZE * zoom) + 1),
	         (int16_t)(y_offset + (float)screen_h / (CELL_SIZE * zoom) + 1));

	for (int y = min.y; y <= std::min((int)max.y, region.dim().y - 2); y++) {
		for (int x = min.x; x <= std::min((int)max.x, region.dim().x - 2); x++) {
			auto& units = unit_manager.get_units(Pos2(x, y));
			for (size_t i = 0; i < units.size(); i++) {
				int min_z = ground_heights[x + y * region.dim().x];
				int max_z = z_level;
				if (units[i]->pos.z >= min_z && units[i]->pos.z <= max_z) {
					float fx = (x - x_offset) * CELL_SIZE;
					float fy = (y - y_offset) * CELL_SIZE;
					new_quad(sf::Vector2f(fx, fy), sf::Color::Black, zoom);
					set_quad_tex(sf::Vector2f(0, 1) * 16.f);
					push_quad(tex_vertices);
				}
			}
		}
	}

	window.draw(&tex_vertices[0], tex_vertices.size(), sf::PrimitiveType::Quads, &tiles);


	if (selecting) {
		sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
		int16_t x = (int16_t)(mouse_pos.x / (CELL_SIZE * zoom) + x_offset);
		int16_t y = (int16_t)(mouse_pos.y / (CELL_SIZE * zoom) + y_offset);
		if (region.in_bounds(Pos2(x, y))) {
			float fx = (x - x_offset) * CELL_SIZE * zoom;
			float fy = (y - y_offset) * CELL_SIZE * zoom;
			sf::RectangleShape rect(sf::Vector2f(CELL_SIZE * zoom, CELL_SIZE * zoom));
			rect.setPosition(fx, fy);
			rect.setFillColor(sf::Color(255, 255, 0, 63));
			window.draw(rect);
			hovering = Pos(x, y, (int16_t)z_level);
		}
	}
}

void Renderer::click() {
	if (selecting) {
		selection_callback(hovering);
		selecting = false;
	}
}

void Renderer::move(Pos2 amount) {
	x_offset += amount.x * move_speed;
	y_offset += amount.y * move_speed;

	int max_x_offset = game.get_region().dim().x - 2 - screen_w / (int)(CELL_SIZE * zoom);
	int max_y_offset = game.get_region().dim().y - 2 - screen_h / (int)(CELL_SIZE * zoom);
	x_offset = std::max(std::min(x_offset, (float)max_x_offset), 1.f);
	y_offset = std::max(std::min(y_offset, (float)max_y_offset), 1.f);
}

void Renderer::shift(int amount) {
	z_level += amount;
	z_level = std::min(std::max(z_level, 1), game.get_region().dim().z - 2);
	update_ground();
}

void Renderer::zoom_in() {
	x_offset += (screen_w / 4.f / CELL_SIZE) / zoom;
	y_offset += (screen_h / 4.f / CELL_SIZE) / zoom;

	zoom *= 2;
	move_speed /= 2;

	move(Pos2(0, 0));
}

void Renderer::zoom_out() {
	if (zoom > 0.25) {
		zoom /= 2;
		move_speed *= 2;

		x_offset -= (screen_w / 4.f / CELL_SIZE) / zoom;
		y_offset -= (screen_h / 4.f / CELL_SIZE) / zoom;
	}

	move(Pos2(0, 0));
}

void Renderer::get_selection(std::function<void(Pos)> callback) {
	selecting = true;
	selection_callback = callback;
}

sf::Color substance_color(uint16_t substance) {
	switch (substance) {
		case 1:  return sf::Color(255, 127, 0);
		case 2:  return sf::Color(127, 127, 127);
		default: return sf::Color(127, 0, 127);
	}
}

int clamp(int val, int min, int max) {
	return std::min(std::max(val, min), max);
}

void Renderer::update_ground() {
	Region& region = game.get_region();
	ground_texture.clear();

	solid_vertices.clear();
	tex_vertices.clear();

	Rando rando(12345);

	for (int y = 1; y <= region.dim().y - 2; y++) {
		for (int x = 1; x <= region.dim().x - 2; x++) {
			int depth = 1;
			uint64_t rand = rando.rand();
			for (int z = z_level; z >= 1; z--, depth++) {
				Pos pos(x, y, z);
				Cell& precell = region.get_cell(pos);
				if (!region.is_walkable(pos)) {
					bool wall = precell.form == Form::Fill || precell.form == Form::Wall;
					if (!(wall && depth == 1 && precell.visible)) {
						continue;
					}
					depth--;
				}

				ground_heights[x + y * region.dim().x] = (int16_t)z;
				if (precell.form == Form::None) {
					pos.z--;
				}
				Cell& cell = region.get_cell(pos);

				sf::Color color;
				if (cell.form == Form::Ramp) {
					color = darken(substance_color(cell.substance), depth - 1);
					color = darken(color, 1, SQRT_DEPTH_FACTOR);
				} else if (cell.form == Form::Floor) {
					color = darken(substance_color(cell.substance), depth - 1);
				} else {
					color = darken(substance_color(cell.substance), depth);
				}

				sf::Vector2f tc(0, 0);
				if (cell.form == Form::Fill) {
					union {
						uint64_t rand;
						uint16_t r[4];
					} u;
					u.rand = rand;

					switch (u.r[0] % 200) {
						case 0: tc.x = 16; break;
						case 1: tc.x = 32; break;
						case 2: tc.x = 48; break;
						case 3: tc.x = 64; break;
						default: break;
					}

					color.r = (uint8_t)clamp(color.r + (int)u.r[1] % 3 - 1, 0, 255);
					color.g = (uint8_t)clamp(color.g + (int)u.r[2] % 3 - 1, 0, 255);
					color.b = (uint8_t)clamp(color.b + (int)u.r[3] % 3 - 1, 0, 255);

				} else if (cell.form == Form::Wall) {
					const unsigned int N = 1;
					const unsigned int S = 2;
					const unsigned int E = 4;
					const unsigned int W = 8;
					unsigned int dirs = 0;
					if (region.get_cell(pos + Pos(0, -1, 0)).form == Form::Wall) {
						dirs |= N;
					}
					if (region.get_cell(pos + Pos(0, 1, 0)).form == Form::Wall) {
						dirs |= S;
					}
					if (region.get_cell(pos + Pos(1, 0, 0)).form == Form::Wall) {
						dirs |= E;
					}
					if (region.get_cell(pos + Pos(-1, 0, 0)).form == Form::Wall) {
						dirs |= W;
					}
					switch (dirs) {
						case 0:             tc = sf::Vector2f(6, 1) * 16.f; break;
						case N:             tc = sf::Vector2f(3, 2) * 16.f; break;
						case S:             tc = sf::Vector2f(2, 2) * 16.f; break;
						case E:             tc = sf::Vector2f(3, 3) * 16.f; break;
						case W:             tc = sf::Vector2f(5, 3) * 16.f; break;
						case N | S:         tc = sf::Vector2f(1, 2) * 16.f; break;
						case N | E:         tc = sf::Vector2f(1, 3) * 16.f; break;
						case N | W:         tc = sf::Vector2f(2, 3) * 16.f; break;
						case S | E:         tc = sf::Vector2f(1, 1) * 16.f; break;
						case S | W:         tc = sf::Vector2f(3, 1) * 16.f; break;
						case E | W:         tc = sf::Vector2f(2, 1) * 16.f; break;
						case N | S | E:     tc = sf::Vector2f(4, 2) * 16.f; break;
						case N | S | W:     tc = sf::Vector2f(5, 1) * 16.f; break;
						case N | E | W:     tc = sf::Vector2f(5, 2) * 16.f; break;
						case S | E | W:     tc = sf::Vector2f(4, 1) * 16.f; break;
						case N | S | E | W: tc = sf::Vector2f(4, 3) * 16.f; break;
						default: break;
					}
				}

				new_quad(sf::Vector2f(x * CELL_SIZE, y * CELL_SIZE), color);
				push_quad(solid_vertices);

				if (tc != sf::Vector2f(0, 0)) {
					new_quad(sf::Vector2f(x * CELL_SIZE, y * CELL_SIZE), darken(color, 1, 0.9));
					set_quad_tex(tc);
					push_quad(tex_vertices);
				}

				break;
			}
		}
	}

	ground_texture.draw(&solid_vertices[0], solid_vertices.size(), sf::PrimitiveType::Quads);
	ground_texture.draw(&tex_vertices[0], tex_vertices.size(), sf::PrimitiveType::Quads, &tiles);
	ground_texture.display();
}

void Renderer::update_menu() {
	Menu& menu = game.get_menu();
	menu_texture.clear();

	for (int i = 0; i < menu.num_options(menu.current()); i++) {
		std::string label = menu.get_label(menu.current(), i);
		char shortcut = menu.get_shortcut(menu.current(), i);
		if (!shortcut) {
			label = "( ) " + label;
		} else {
			label = "(" + std::string(1, shortcut) + ") " + label;
		}

		sf::Text text(label, font, 16);

		text.setColor(sf::Color::White);
		text.setPosition(16, 16 + i * 32);
		menu_texture.draw(text);
	}

	sf::Text text(game.is_paused() ? "Paused" : "Running", font, 16);
	text.setColor(sf::Color::White);
	text.setPosition(16, screen_h - 32);

	menu_texture.draw(text);
	menu_texture.display();
}

void Renderer::new_quad(sf::Vector2f pos, sf::Color color, float zoom) {
	vertex00.position = pos * zoom;
	vertex10.position = vertex00.position + sf::Vector2f(CELL_SIZE * zoom, 0);
	vertex01.position = vertex00.position + sf::Vector2f(0, CELL_SIZE * zoom);
	vertex11.position = vertex00.position + sf::Vector2f(CELL_SIZE * zoom, CELL_SIZE * zoom);

	vertex00.color = color;
	vertex01.color = color;
	vertex10.color = color;
	vertex11.color = color;
}

void Renderer::set_quad_tex(sf::Vector2f tex_coords) {
	vertex00.texCoords = tex_coords;
	vertex10.texCoords = tex_coords + sf::Vector2f(16.f, 0);
	vertex01.texCoords = tex_coords + sf::Vector2f(0, 16.f);
	vertex11.texCoords = tex_coords + sf::Vector2f(16.f, 16.f);
}

void Renderer::push_quad(std::vector<sf::Vertex>& vertices) {
	vertices.push_back(vertex00);
	vertices.push_back(vertex01);
	vertices.push_back(vertex11);
	vertices.push_back(vertex10);
}
