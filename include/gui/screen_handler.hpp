#ifndef SCREEN_HANDLER_HPP
#define SCREEN_HANDLER_HPP

#include "data/database_manager.hpp"
#include "data/data_structures.hpp"
#include "gui/window_manager.hpp"
#include <ncurses.h>
#include <string>

using namespace std;

class screen_handler
{
	int y;
	int x;

public:
	window_manager			wm;

	screen_handler(database_manager& dm)
	:	wm(dm) {};
	void initialize();
	void setup_ncurses();
	void initialize_colors();
	void grab_dimensions();

	void resize();
	bool consider_resize();

	void set_status(string s) {wm.set_status(s);}
};

#endif