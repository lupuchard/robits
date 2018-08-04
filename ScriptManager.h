#ifndef TOWN_SCRIPTMANAGER_H
#define TOWN_SCRIPTMANAGER_H


#include "Unit.h"

class ScriptManager {
public:
	enum Event {
		Timer,
		NewJob,
	};

	void load_default_scripts();

	void run_script(Unit& unit, Event event);

private:

};


#endif //TOWN_SCRIPTMANAGER_H
