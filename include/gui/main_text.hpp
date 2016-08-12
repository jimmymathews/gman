#ifndef MAIN_TEXT_HPP
#define MAIN_TEXT_HPP

#include "gui/text_window.hpp"
#include "gui/category_bar.hpp"
#include "gui/status_bar.hpp"
#include "data/database_manager.hpp"
#include "data_interaction/tree_viewer.hpp"
#include <vector>
#include <ncurses.h>
using namespace std;

class main_text : public text_window
{
	category_bar&		cb;
	status_bar&			sb;

public:
	tree_viewer	tv;
	main_text(category_bar& cb, status_bar& sb, database_manager& dm)
	: cb(cb), sb(sb), tv(dm,sb) {};
	void initialize(int termy, int termx) override { make(termy, termx);};
	void make(int termy, int termx)
	{
		y = termy-2;
		x = termx;
		starty = 1;
		startx = 0;

		if(!cb.is_active())
		{
			y = termy -1;
			starty = 0;
		}

		use_default_colors();
		w = newwin(y, x, starty, startx);
		wtimeout(w, -1);
	};

	void draw()
	{
		werase(w);
		tv.write_on(w, y, x);
		wrefresh(w);
	};
	void refresh_data()
	{
		cb.refresh_data();
		tv.refresh_data();
	};

	void up()			{ tv.up();};
	void down() 		{ tv.down();};
	void right() 		{ tv.right();draw();};
	void left() 		{ tv.left();};
	void shift_up() 	{ tv.shift_up();};
	void shift_down() 	{ tv.shift_down();};
	void shift_left() 	{ tv.shift_left();};
	void shift_right() 	{ tv.shift_right();};
	void enter() 		{ tv.enter(); };
	void ctrl_v() 		{ tv.ctrl_v(); };
	void ctrl_c() 		{ tv.ctrl_c(); };
	void ctrl_g()		{ tv.ctrl_g();};
	void ctrl_e()		{ tv.ctrl_e();};
	void home()			{ tv.home(); };
	void end()			{ tv.end(); };
	bool handle_delete(){ return tv.handle_delete();};
	void alphanumeric(int ch){ tv.alphanumeric(ch);};
	void backspace()	{ tv.backspace();};

	void get_focus()
	{
		text_window::get_focus();
		tv.get_focus();
	};
	void lose_focus()
	{
		text_window::lose_focus();
		tv.lose_focus();
	};
};

#endif