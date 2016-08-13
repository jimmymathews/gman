#ifndef BIG_NODE_EDITOR_HPP
#define BIG_NODE_EDITOR_HPP

#include "config_p.hpp"
#include "node_writer.hpp"

class big_node_editor
{
node* focal_node = NULL;
public:
	node_writer& nw;
	big_node_editor(node_writer& nw) : nw(nw) {};

	void write(node* n, int start, int end, bool selecting)
	{
		focal_node = n;
		nw.clear();

		int print_width = nw.width();
		string& content = n->get_contents();
		int contents_size = content.length();				//currently print_width has to reach all the way to the end of the screen; no newlines are added
		if(print_width <= 0)
			return;
		int whole_lines = contents_size / print_width;

		n->get_node_type().turn_on_color(nw.window());

		int number_of_lines_to_show = start/print_width + nw.height()/2;
		if(number_of_lines_to_show < nw.height())
			number_of_lines_to_show = min(whole_lines,nw.height()-1);

		if( whole_lines - (start/print_width) < nw.height()/2)
			number_of_lines_to_show = whole_lines;

		

		scrollok(nw.window(), true);
		for(int i=0; i < number_of_lines_to_show; i++)
		{
			nw.print_fancy_editing_string(content.substr(i*print_width,print_width), start, end, selecting);
		}

		if(number_of_lines_to_show == whole_lines)
		{
			nw.print_fancy_editing_string(content.substr(number_of_lines_to_show*print_width, contents_size - number_of_lines_to_show*print_width), start, end, selecting);
		}

		if(start == contents_size && !selecting)
		{
			if(nw.cursor_x() == nw.width())
			{
				nw.carriage();
			}
			wattron(nw.window(),A_UNDERLINE);
			waddstr(nw.window()," ");
			wattroff(nw.window(),A_UNDERLINE);
		}

		n->get_node_type().turn_off_color(nw.window());
	};
};

#endif