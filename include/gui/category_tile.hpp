#ifndef CATEGORY_TILE_HPP
#define CATEGORY_TILE_HPP

#include "data/data_structures.hpp"
#include <ncurses.h>
#include <string>
using namespace std;

class category_tile
{
	bool focus = false;

public:
	node_type& nt;
	bool is_being_edited = false;

	category_tile(node_type& nt) : nt(nt) {};
	void write_on_with_size(WINDOW* w, int n, bool blinky);
	void write_on(WINDOW* w, bool blinky);
	void write_string(WINDOW* w, string s, bool blinky);
	void toggle_color();
	int length()		{return nt.get_name().length();};
	void get_focus()	{focus = true;};	
	void lose_focus()	{focus = false;};
	bool has_focus()	{return focus;};	
	void toggle_enabled(){nt.toggle_enabled();};
};

#endif