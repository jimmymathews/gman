#ifndef STATUS_BAR_HPP
#define STATUS_BAR_HPP

#include "data/data_structures.hpp"
#include "gui/text_window.hpp"
#include <ncurses.h>
#include <thread>
#include <chrono>
#include <string>
#include <sstream>

using namespace std;

class status_bar : public text_window
{
	string status = "";	
	string cwd;
	string current_file="";
	string pushed_filename = "";

	bool linking = false;
	node* linkee = NULL;
	node* linko = NULL;
	bool more_than_one_linko = false;

public:
	status_bar(string cwd) : cwd(cwd)
	{
		// cwd = get_working_path();
		status = cwd;
	};
	void initialize(int termy, int termx) override { make(termy, termx);};

	void first_time_opened(string f)
	{
		status = cwd + "/" + f;
		current_file = cwd + "/" + f;
	};

	void restore_current_filename()
	{
		current_file = pushed_filename;
		set_status(current_file);
	};
	
	void make(int termy, int termx) override
	{
		y = 1;
		x = termx;
		starty = termy - 1;
		startx = 0;
		
		use_default_colors();
		w = newwin(y, x, starty, startx);
		wtimeout(w, -1);
	};

	void draw() override
	{
		if(!linking)
		{
			int width; int height;
			getmaxyx(w,height,width);
			string bit = status;
			if(status.size() >= width)
				bit = status.substr(status.size()-width, width);
			werase(w);
			mvwaddstr(w, 0, 0, bit.c_str());
			wrefresh(w);
		}
		else
		{
			werase(w);
			wmove(w, 0,0);
			if(more_than_one_linko)
				waddstr(w, "and ");
			waddstr(w, "link ");

			int ssize = 25;
			if(ssize > x/2)
				ssize = x/2;

			string c = linkee->get_contents();
			linkee->get_node_type().turn_on_color(w);
			waddstr(w, c.substr(0,ssize).c_str());
			if(c.length()>ssize)
				waddstr(w,"..");
			linkee->get_node_type().turn_off_color(w);


			waddstr(w, " to ");

			if(linko != NULL)
			{
				string co = linko->get_contents();
				linko->get_node_type().turn_on_color(w);
				waddstr(w, co.substr(0,ssize).c_str());
				if(co.length()>ssize)
					waddstr(w,"..");
				linko->get_node_type().turn_off_color(w);
			}
			wrefresh(w);
		}
	};

	void notify_of_new_linko(node* l, int number_of_linkos)
	{
		linko = l;		
		if(number_of_linkos>1)
			more_than_one_linko = true;
	}

	void set_status(string s)
	{
		linking = false;
		status = s;
		draw();
	};

	void set_temporary_status(string s)
	{
		linking = false;
		string pushed = status;
		status = s;
		draw();

		std::this_thread::sleep_for(std::chrono::milliseconds(1500));
		status = pushed;
		draw();
	}

	string save_dialog()
	{
		pushed_filename = current_file;
		linking = false;
		current_file = obtain_file_name("save ", "/g", ".graphml");
		set_status(current_file);
		return current_file;
	};

	string open_dialog()
	{
		pushed_filename = current_file;
		linking = false;
		current_file = obtain_file_name("open ", "/", ".graphml");
		set_status(current_file);
		return current_file;
	};

	string obtain_file_name(string addend, string def, string caboose)
	{
		werase(w);
		string buffer = addend + cwd + def + caboose;
		if(current_file != "")
			buffer = addend + current_file;

		set_status(buffer);

		int width; int height;
		getmaxyx(w,height,width);

		int x = addend.length() + cwd.length() + def.size();
		if(current_file != "")
			x = addend.length() + current_file.length() - caboose.size();
		highlight_character(x);
		int ch = wgetch(w);
		while(ch!=10 && ch !=13)
		{
			if(ch==27)
			{
				set_status(current_file);
				set_temporary_status("cancelled");
				return "";
			}
			// if(is_alphanumeric(ch) && buffer.size() < width)
			if(is_alphanumeric(ch) )
			{
				string addend(1,ch);
				buffer.insert(x,addend);
				x++;
			}
			else
			if(ch == 127 && x>addend.size())	//backspace
			{
				buffer.erase(x-1, 1);
				x--;
			}
			set_status(buffer);
			highlight_character(x);
			ch = wgetch(w);
		}
		return buffer.substr(addend.size(), buffer.size() - addend.size());
	};

	bool is_alphanumeric(int ch) { return (32 <= ch && ch <= 126); };

	void highlight_character(int x)
	{
		int width; int height;
		getmaxyx(w,height,width);
		int y = x;
		if(status.size() >= width)
			y = x - (status.size() - width);

		wattron(w, A_REVERSE);
		char local_c = mvwinch(w, 0, y);
		mvwaddch(w, 0, y, local_c);
		wattroff(w, A_REVERSE);
	};

	void use_link_dialog(node* l)
	{
		linking = true;
		linkee = l;
		linko = NULL;
	}

	void* stop_linking()
	{
		linking = false;
		more_than_one_linko = false;
	}
};

#endif