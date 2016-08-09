#ifndef NODE_WRITER_HPP
#define NODE_WRITER_HPP

#include "config_p.hpp"
#include <ncurses.h>

class node_writer
{
	WINDOW* win;
	int h;
	int w;
	int tab_size;

public:
	node_writer() {tab_size = config_p::tab_size;};

	void write_link(int max_relation_length, directed_link* dl,int depth)
	{
		int print_size = w-tab_size*depth;
		if(print_size <=0 )
			return;
	
		node* n = dl->get_end_node();
		string& c = n->get_contents();
		string rd= pad_string_to(max_relation_length,dl->get_name());

		waddstr(win,string(tab_size*depth,' ').c_str());

		if(rd != ""  && rd.length() < (print_size -3))
		{
			print_size = print_size - max_relation_length -2;
			waddstr(win,(rd+": ").c_str());
		}

		string to_print;
		if(c.length() == print_size)
			to_print = c;
		else
		{
			to_print = c.substr(0, print_size);
			if(c.length()>to_print.length())
				to_print = to_print.substr(0,to_print.length()-2) + "..";
		}
			
		n->turn_on_formatting(win);
		waddstr(win, to_print.c_str() );
		n->turn_off_formatting(win);
		
		if(cursor_x()!=0)
			carriage();
	};

	void write_to_end_of_line(node* n,int depth)
	{
		int print_size = w-tab_size*depth;
		if(print_size <=0 )
			return;
	
		string c = n->get_contents();
		string to_print;
		if(c.length() == print_size)
			to_print = c;
		else
		{
			to_print = string(tab_size*depth,' ') + c.substr(0, w-tab_size*depth);
			if(c.length()>to_print.length())
				to_print = to_print.substr(0,to_print.length()-2) + "..";
		}
			
		n->turn_on_formatting(win);
		waddstr(win, to_print.c_str() );
		n->turn_off_formatting(win);
		
		if(cursor_x()!=0)
			carriage();
	};


	string pad_string_to(int max, string in)
	{
		if(in.length() > max)
			return "ERR: link pad length";
		return in + string(max-in.length(),' ');
	};

	void carriage()
	{
		if(cursor_y() < h-1)
			wmove(win, cursor_y()+1,0); 
		else
		{
			wscrl(win,1);
			wmove(win,h-1,0);
		}
	};

	int cursor_y()
	{
		int x;
		int y;
		getyx(win, y,x);
		return y;
	};

	int cursor_x()
	{
		int x;
		int y;
		getyx(win, y,x);
		return x;
	};

	string node_summary(node* n)
	{
		int remaining_width = w-1- 5;
		int print_width = remaining_width;
		string ender = "";
		bool trim = (n->contents().size() > 10);
		if(trim)
		{
			print_width = 10;
			ender = ".. ";
		}
		return ((n->contents()).substr(0, print_width ) + ender);
	};

	void initialize(WINDOW* window, int height, int width)
	{
		win = window;
		h = height;
		w = width;
	};
};

#endif