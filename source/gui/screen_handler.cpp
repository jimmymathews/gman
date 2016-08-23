#include "gui/screen_handler.hpp"

#include <fstream>
using namespace std;

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

string screen_handler::home_directory()
{
	string dir = getenv("HOME");	//Linux only?
	return dir + "/";
}

bool screen_handler::file_exists(string name)
{ 
	ifstream f(name.c_str());
    if (f.good())
    {
        f.close();
        return true;
    } else
    {
        f.close();
        return false;
    }
}

void screen_handler::grab_dimensions()
{
	string name = home_directory()+".gman";
	if(file_exists(name))
	{
		ifstream settings(name, ios_base::in);
		int Y=0;
		int X=0;
		settings >> Y;
		settings >> X;
		if(Y<10000 && X<10000 && Y>2 && X>2)
			y=Y;
			x=X;
	}
	else
	{
		y = LINES;
		x = COLS;
	}
}

void screen_handler::resize()
{
	endwin();
	refresh();
	grab_dimensions();
	// y = LINES;
	// x = COLS;
	wm.resize(y,x);
}

bool screen_handler::consider_resize()
{
	int px=x;
	int py=y;
	grab_dimensions();
	if(px!=x || py!=y)
	{
		resize();
		return true;
	}
	return false;

	// if(y != LINES || x != COLS)
	// {
	// 	resize();
	// 	return true;
	// }
	// return false;
}




