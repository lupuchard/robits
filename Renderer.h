#ifndef TOWN_RENDERER_H
#define TOWN_RENDERER_H

#include "Region/Region.h"
#include "Menu.h"
#include "Game.h"
#include <SFML/Graphics.hpp>

class Renderer: public IRenderer {
public:
	Renderer(sf::RenderWindow& window, Game& game);

	void click();
	void move(Pos2 amount);
	void shift(int amount);
	void render();

	void update_ground() override;
	void update_menu() override;

	void zoom_in();
	void zoom_out();

	void get_selection(std::function<void(Pos)> callback) override;

private:
	void new_quad(sf::Vector2f pos, sf::Color color, float zoom = 1.f);
	void set_quad_tex(sf::Vector2f tex_coords);
	void push_quad(std::vector<sf::Vertex>& vertices);

	sf::RenderWindow& window;
	Game& game;

	int z_level;
	float zoom = 1;
	float move_speed = 0.5f;
	float x_offset, y_offset;
	int screen_w, screen_h;
	std::vector<sf::Vertex> solid_vertices;
	std::vector<sf::Vertex> tex_vertices;

	sf::Font font;
	sf::Texture tiles;

	std::unique_ptr<int16_t[]> ground_heights;
	sf::RenderTexture ground_texture;
	sf::RenderTexture menu_texture;
	sf::Vertex vertex00, vertex01, vertex10, vertex11;

	bool selecting = false;
	Pos hovering;
	std::function<void(Pos)> selection_callback;
};

#endif //TOWN_RENDERER_H
