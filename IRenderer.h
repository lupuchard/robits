#ifndef TOWN_IRENDERER_H
#define TOWN_IRENDERER_H

#include "Region/Pos.h"
#include <functional>

class IRenderer {
public:
	virtual void update_ground() = 0;
	virtual void update_menu() = 0;

	virtual void get_selection(std::function<void(Pos)> callback) = 0;
};

#endif //TOWN_IRENDERER_H
