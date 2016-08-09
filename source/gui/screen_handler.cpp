#include "gui/screen_handler.hpp"

void screen_handler::initialize()
{
	setup_ncurses();
	grab_dimensions();
	wm.initialize(y,x);
	wm.refresh_data();
	resize();
}

void screen_handler::setup_ncurses()
{
	initscr();
	initialize_colors();
	cbreak();
	raw();
	noecho();
	curs_set(0);
}

void screen_handler::initialize_colors()
{
	if(has_colors())
	{
		start_color();
		use_default_colors();
	}
	for(int i=0; i<8; i++)
		init_pair(i,i, -1);
}

void screen_handler::grab_dimensions()
{
	y = LINES;
	x = COLS;
}


void screen_handler::resize()
{
	endwin();
	refresh();
	y = LINES;
	x = COLS;
	wm.resize(y,x);
}

bool screen_handler::consider_resize()
{
	if(y != LINES || x != COLS)
	{
		resize();
		return true;
	}
	return false;
}




