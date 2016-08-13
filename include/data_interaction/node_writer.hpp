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

	int position_counter = 0;

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
			print_fancy_string(rd+": ");
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
		print_fancy_string(to_print);
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
		print_fancy_string(to_print);
		n->turn_off_formatting(win);
		
		if(cursor_x()!=0)
			carriage();
	};

	void write_editing_node(node* n, int vertical_offset, int horizontal_offset, int print_width, int start, int end, bool selecting)
	{
		string& content = n->get_contents();
		int contents_size = content.length();				//currently print_width has to reach all the way to the end of the screen; no newlines are added
		if(print_width <= 0)
			return;
		int whole_lines = contents_size / print_width;

		position_counter = 0;
		scrollok(win,false);

		n->get_node_type().turn_on_color(win);

		for(int i=0; i < whole_lines; i++)
		{
			print_pad(horizontal_offset);
			print_fancy_editing_string(content.substr(i*print_width,print_width), start, end, selecting);
		}

		int last_position = print_width * whole_lines;
		if(position_counter == last_position)
		{
			print_pad(horizontal_offset);
			print_fancy_editing_string(content.substr(last_position, contents_size-last_position), start, end, selecting);
		}


		if(start == contents_size && !selecting)
		{
			if(cursor_x() == w)
			{
				carriage();
				print_pad(horizontal_offset);
			}
			wattron(win,A_UNDERLINE);
			waddstr(win," ");
			wattroff(win,A_UNDERLINE);
			carriage();
		}
		else
		{		
			carriage();
		}

		n->get_node_type().turn_off_color(win);
		scrollok(win,true);
	};

	void print_pad(int horizontal_offset)
	{
		waddstr(win,string(horizontal_offset,' ').c_str());
	};

	void print_fancy_string(string s)
	{
		for(int i=0; i<s.length(); i++)
		{
			int c = s.at(i);
			print_fancy_character(c);
		}
	};

	void print_fancy_editing_string(string s, int start, int end, bool selecting)
	{
		for(int i=0; i<s.length(); i++)
		{
			int c = s.at(i);
			print_fancy_editing_character(c, start, end, selecting);
			position_counter++;
		}
	};

	void print_fancy_editing_character(int ch, int start, int end, bool selecting)
	{

		if(selecting)
		{
			if(position_counter >= start && position_counter < end)
				wattron(win,A_REVERSE);
		}
		else
		{
			if(position_counter == start)
				wattron(win,A_UNDERLINE);
		}

		print_fancy_character(ch);

		if(selecting)
		{
			if(position_counter >= start && position_counter < end)
				wattroff(win,A_REVERSE);
		}
		else
		{
			if(position_counter == start)
				wattroff(win,A_UNDERLINE);
		}
	};

	void print_fancy_character(int ch)
	{
		//normal ascii
		char cc =static_cast<char>(ch);
		if(32 <= cc && cc<= 126)
		{
			string s = string(1,cc);
			waddstr(win,s.c_str());
			return;
		}

		if(ch == 10)
		{
			if(cursor_x()!=w-1)
				waddstr(win," ");
			carriage();
			return;
		}

		unsigned char c = ch;

		//superscipts
		if(c==24)
			waddstr(win,"\u207A"); //+ 43 ascii
		if(c==25)
			waddstr(win,"\u207B"); //- 45 ascii
		//numerals
		if(c==14)
			waddstr(win,"\u2070");	//0
		if(c==15)
			waddstr(win,"\u00B9");	//..
		if(c==16)
			waddstr(win,"\u00B2");
		if(c==17)
			waddstr(win,"\u00B3");
		if(c==18)
			waddstr(win,"\u2074"); //4..
		if(c==19)
			waddstr(win,"\u2075");
		if(c==20)
			waddstr(win,"\u2076");
		if(c==21)
			waddstr(win,"\u2077");
		if(c==22)
			waddstr(win,"\u2078");
		if(c==23)
			waddstr(win,"\u2079");

		//subscripts
		if(c==27)
			waddstr(win,"\u208A"); //+ 43 ascii
		if(c==28)
			waddstr(win,"\u208B"); //- 45 ascii

		//numerals
		if(c==128)
			waddstr(win,"\u2080");	//0
		if(c==129)
			waddstr(win,"\u2081");	//..
		if(c==130)
			waddstr(win,"\u2082");
		if(c==131)
			waddstr(win,"\u2083");
		if(c==132)
			waddstr(win,"\u2084"); //4..
		if(c==133)
			waddstr(win,"\u2085");
		if(c==134)
			waddstr(win,"\u2086");
		if(c==135)
			waddstr(win,"\u2087");
		if(c==136)
			waddstr(win,"\u2088");
		if(c==137)
			waddstr(win,"\u2089");

		//nonnumerals
		if(c==(138-97)+'a')
			waddstr(win,"\u2090");	//a 
		if(c==(138-97)+'e')
			waddstr(win,"\u2091");	//e
		if(c==(138-97)+'o')
			waddstr(win,"\u2092");	//o
		if(c==(138-97)+'x')
			waddstr(win,"\u2093");	//x
		if(c==(138-97)+'h')
			waddstr(win,"\u2095");	//h
		if(c==(138-97)+'k')
			waddstr(win,"\u2096");	//k
		if(c==(138-97)+'l')
			waddstr(win,"\u2097");	//l
		if(c==(138-97)+'m')
			waddstr(win,"\u2098");	//m
		if(c==(138-97)+'n')
			waddstr(win,"\u2099");	//n
		if(c==(138-97)+'p')
			waddstr(win,"\u209A");	//p
		if(c==(138-97)+'s')
			waddstr(win,"\u209B");	//s
		if(c==(138-97)+'t')
			waddstr(win,"\u209C");	//t

		if(193<=c && c<=250)
		{
			unsigned char latin_version = c-128;
			if(latin_version == 'a')	//alpha
				waddstr(win,"\u03B1");
			if(latin_version == 'A')	//alpha
				waddstr(win,"A");

			if(latin_version == 'b')	//beta
				waddstr(win,"\u03B2");
			if(latin_version == 'B')	//beta
				waddstr(win,"B");

			if(latin_version == 'c')	//psi
				waddstr(win,"\u03C8");
			if(latin_version == 'C')	//psi
				waddstr(win,"\u03A8");

			if(latin_version == 'd')	//delta
				waddstr(win,"\u03B4");
			if(latin_version == 'D')	//delta
				waddstr(win,"\u0394");

			if(latin_version == 'e')	//epsilon
				waddstr(win,"\u03B5");
			if(latin_version == 'E')	//epsilon
				waddstr(win,"E");

			if(latin_version == 'f')	//phi
				waddstr(win,"\u03C6");
			if(latin_version == 'F')	//phi
				waddstr(win,"\u03A8");

			if(latin_version == 'g')	//gamma
				waddstr(win,"\u03B3");
			if(latin_version == 'G')	//gamma
				waddstr(win,"\u0393");

			if(latin_version == 'h')	//theta
				waddstr(win,"\u03B8");
			if(latin_version == 'H')	//theta
				waddstr(win,"\u0398");

			if(latin_version == 'i')	//iota
				waddstr(win,"\u03B9");
			if(latin_version == 'I')	//iota
				waddstr(win,"I");
			
			if(latin_version == 'j')	//
				waddstr(win,"j");
			if(latin_version == 'J')	//
				waddstr(win,"J");

			if(latin_version == 'k')	//kappa
				waddstr(win,"\u03BA");
			if(latin_version == 'K')	//kappa
				waddstr(win,"K");

			if(latin_version == 'l')	//lambda
				waddstr(win,"\u03BB");
			if(latin_version == 'L')	//lambda
				waddstr(win,"\u039B");

			if(latin_version == 'm')	//mu
				waddstr(win,"\u03BC");
			if(latin_version == 'M')	//mu
				waddstr(win,"M");

			if(latin_version == 'n')	//nu
				waddstr(win,"\u03BD");
			if(latin_version == 'N')	//nu
				waddstr(win,"N");

			if(latin_version == 'o')	//omega
				waddstr(win,"\u03C9");
			if(latin_version == 'O')	//omega
				waddstr(win,"\u03A9");

			if(latin_version == 'p')	//pi
				waddstr(win,"\u03C0");
			if(latin_version == 'P')	//pi
				waddstr(win,"\u03A0");

			if(latin_version == 'q')	//eta
				waddstr(win,"\u03B7");	
			if(latin_version == 'Q')
				waddstr(win,"Q");	
			
			if(latin_version == 'r')	//rho
				waddstr(win,"\u03C1");
			if(latin_version == 'R')	//rho
				waddstr(win,"P");

			if(latin_version == 's')	//sigma
				waddstr(win,"\u03C3");
			if(latin_version == 'S')	//sigma
				waddstr(win,"\u03A3");

			if(latin_version == 't')	//tau
				waddstr(win,"\u03C4");
			if(latin_version == 'T')	//tau
				waddstr(win,"T");
			if(latin_version == 'u' || latin_version == 'U')	
				waddstr(win,"U");
			if(latin_version == 'v' || latin_version == 'V')	
				waddstr(win,"V");
			if(latin_version == 'w' || latin_version == 'W')	
				waddstr(win,"W");
			if(latin_version == 'x')	//chi
				waddstr(win,"\u03C7");
			if(latin_version == 'X')	//chi
				waddstr(win,"X");
			if(latin_version == 'y' || latin_version == 'Y')
				waddstr(win,"Y");
			if(latin_version == 'z')	//zeta
				waddstr(win,"\u03B6");
			if(latin_version == 'Z')	//zeta
				waddstr(win,"Z");
		}
	};

	string pad_string_to(int max, string in)
	{
		if(in.length() > max)
			return "ERR: link pad length";
		return in + string(max-in.length(),' ');
	};

	void carriage()
	{
		// if(cursor_y() < h)
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

	int width()
	{
		return w;
	};

	int height()
	{
		return h;
	};

	WINDOW* window()
	{
		return win;
	};

	void clear()
	{
		position_counter = 0;
		wclear(win);
		wmove(win,0,0);
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