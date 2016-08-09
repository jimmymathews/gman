#ifndef NODE_EDITOR_HPP
#define NODE_EDITOR_HPP

#include "config_p.hpp"

class node_editor
{
	//State
	WINDOW* win;
	int h;
	int w;
	bool editing = false;
	bool selecting = false;
	node* focal_node = NULL;
	int tab_size;
	int contents_pos = 0;
	int mark_pos = -1;
	int print_width;
	vector<node*> focal_selection_history;


	//Selection-related	
	int horizontal_offset;
	int vertical_offset;

	int  last_contents_position=-1;
	int  last_mark_position=-1;
	int  temporary_cursor_x;
	int  temporary_cursor_y;
	string clip_cache;

public:
	// node_editor () : nw(win,h,w,tab_size) {};
	node_editor() {tab_size = config_p::tab_size;};

	//State
	void initialize(WINDOW* window, int height, int width)
	{
		win = window;
		h = height;
		w = width;
	};

	void refresh_data()
	{
		stop_editing();
	};

	bool stop_editing()
	{
		if(focal_node == NULL)
			return false;
		editing = false;
		selecting = false;
		contents_pos = 0;
		mark_pos = -1;


		string combo = focal_node->get_contents();
		string& contents_reference = focal_node->get_contents();
		contents_reference = get_new_contents(combo);
	
		int n = focal_selection_history.size();		
		if(n>1)
		{
			node* parent = focal_selection_history[n-2];
			node* child = focal_selection_history[n-1];
			directed_link* dl = NULL;
			for(int i=0; i<parent->get_links().size(); i++)
			{
				directed_link* li = parent->get_links()[i];
				if(li->get_end_node() == child)
					dl = li;
			}
			
			if(dl != NULL) //assert..
			{
				dl->set_name(get_new_relation_descriptor(combo));
				parent->update_max_relation_length();
			}
			
		}
		
		
		if(focal_node->get_contents() == "")
		{
			focal_node = NULL;
			return true;
		}
		return false;
	};

	void start_editing(vector<node*> fsh)
	{
		int n = fsh.size();
		if(n == 0)
			return;
		focal_selection_history = fsh; //?
		int depth = fsh.size()-1;
		print_width = w - (depth*tab_size);
		editing = true;
		contents_pos = 0;

		node* child = fsh[n-1];
		string& contents_combo = child->get_contents();

		if(n>1)
		{
			node* parent = fsh[n-2];
			directed_link* dl = NULL;
			for(int i=0; i<parent->get_links().size(); i++)
			{
				directed_link* li = parent->get_links()[i];
				if(li->get_end_node() == child)
					dl = li;
			}
			
			if(dl != NULL) //assert..
			{
				if(dl->get_name() != "")
				{
					contents_combo = dl->get_name() + ": " + contents_combo;
					contents_pos = dl->get_name().length() + 2;
				}
			}	
		}
	};

	bool is_editing() { return editing;};

	bool is_editing_specific_element(vector<node*> selection_history)
	{
		if(!editing)
			return false;
		for(int i=0; i<selection_history.size();i++)
		{
			if(focal_selection_history[i]!=selection_history[i])
				return false;
		}
		return true;
	};

	string get_new_relation_descriptor(string& edited)
	{
		size_t delimiter_position = edited.find(": ");
		if(delimiter_position < w && delimiter_position >=0)
		{
			string rd = edited.substr(0,delimiter_position);
			if(rd != string(rd.length(),' '))
				return edited.substr(0,delimiter_position);
		}
		return "";
	};

	string get_new_contents(string& edited)
	{
		size_t delimiter_position = edited.find(": ");
		if(delimiter_position < w && delimiter_position >=0)
			return edited.substr(delimiter_position+2, edited.length()-delimiter_position-2);
		else
			return edited;
	};



	//Writing
	void write_editing_node(node* n)
	{
		focal_node = n;
		string content = focal_node->get_contents();// string& contents_reference = focal_node->get_contents();
		int depth = focal_selection_history.size() -1;
		int contents_size = content.length();
		vertical_offset = cursor_y();
		horizontal_offset = tab_size*depth;
		if(print_width <= 0)
			return;
		int whole_lines = contents_size / print_width;

		focal_node->get_node_type().turn_on_color(win);
		for(int i=0; i < whole_lines; i++)
		{
			waddstr(win, (string(horizontal_offset,' ') + content.substr(i*print_width,print_width)).c_str() );
			if( vertical_offset + i == h-1) //test this
				return;
		}

		int last_position = print_width * whole_lines;
		waddstr(win, (string(tab_size*depth,' ') + content.substr(last_position, contents_size) ).c_str());
		carriage();

		if(selecting)
		{
			mark_selection();
		}
		else
		{
			push_cursor(win);  //prob not necessary
			int local_editor_y = (contents_pos / print_width) + vertical_offset;
			int local_editor_x =(contents_pos % (print_width) + horizontal_offset );
			char local_c = mvwinch(win, local_editor_y , local_editor_x);
			wattron(win, A_UNDERLINE);
			mvwaddch(win, local_editor_y, local_editor_x, local_c);
			wattroff(win, A_UNDERLINE);
			pop_cursor(win);  //
		}

		focal_node->get_node_type().turn_off_color(win);
	};

	void carriage()
	{
		if(cursor_y() < h-1)
			wmove(win, cursor_y()+1,0); 
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
		return y;
	};


	//Navigation
	void navigation_action(string direction)
	{
		if(selecting)
			selecting=false;
		basic_action(direction);
	};

	void shift_navigation_action(string direction)
	{
		if(!selecting)
		{
			selecting=true;
			mark_pos = contents_pos;
		}
		basic_action(direction);
	};

	void basic_action(string direction)
	{
		string& contents_reference = focal_node->get_contents();
		int contents_size = contents_reference.size();

		if(direction == "left")
		{
			if(contents_pos >0)
				contents_pos--;
		}
		if(direction == "right")
		{
			if(contents_pos <contents_size)
				contents_pos++;
		}
		if(direction == "up")
		{
			if(contents_pos >= print_width)
				contents_pos = contents_pos - print_width;
		}
		if(direction == "down")
		{
			if(contents_pos + print_width < contents_size+1)
				contents_pos = contents_pos + print_width;
		}
	};

	void home()
	{
		contents_pos  = (contents_pos / print_width) * print_width;
	};

	void end()
	{
		string& contents_reference = focal_node->get_contents();
		int contents_size = contents_reference.length();

		if(contents_pos>=contents_size-1)
			contents_pos = contents_size;
		else
		{
			int red = contents_pos / print_width;
			contents_pos = (red+1)*print_width -1;
		}
		if(contents_pos>=contents_size-1)
			contents_pos = contents_size;		
	};


	//Deletion
	bool handle_delete()
	{
		string& contents_reference = focal_node->get_contents();
		int contents_position = contents_pos;
		int contents_size = contents_reference.length();
		if(selecting)
		{
			int start= selection_start();
			int end = selection_end();

			contents_reference.erase(start, end-start);
			contents_pos = start;
			selecting=false;
		}
		else
		{
			contents_reference.erase(contents_position,1);
		}
		return false;
	};

	void backspace()
	{
		string& contents_reference = focal_node->get_contents();
		int contents_position = contents_pos;
		int contents_size = contents_reference.length();
		if(selecting)
		{
			int start= selection_start();
			int end = selection_end();

			contents_reference.erase(start, end-start);
			contents_pos = start;
			selecting=false;
		}
		else
		{
			if(contents_position>0)
			{
				contents_reference.erase(contents_position-1,1);
				contents_pos--;
			}
		}
	};



	//Insertion
	void alphanumeric(int ch)
	{
		char char_cache = ch;
		string str1char(1,char_cache);
		string& contents_reference = focal_node->get_contents();
		int contents_position = contents_pos;
		int start = contents_position;
		if(selecting)
		{
			start= selection_start();
			int end = selection_end();
			contents_reference.erase(start, end-start);
			contents_pos = start;
			selecting=false;
		}
		contents_reference.insert(start, str1char );
		contents_pos= start+1;
	};

	void clip(){ copy_to_my_clip(get_selection());};

	void copy_to_my_clip(string in){ clip_cache = in;};

	void paste()
	{
		string& contents_reference = focal_node->get_contents();
		int contents_position = contents_pos;
		int start = contents_position;
		if(selecting)
		{
			start= selection_start();
			int end = selection_end();
			contents_reference.erase(start, end-start);
			contents_pos = start;
			selecting=false;
		}
		contents_reference.insert(start, clip_cache );
		contents_pos = start +1 + clip_cache.length()-1;
	};

	
	//Selection
	void mark_selection()
	{
		push_cursor(win);
		wattron(win, A_REVERSE);

		for(int i = selection_start(); i<selection_end(); i++)
		{
			int y = get_y_of_contents(i);
			int x = get_x_of_contents(i);
			char local_c = mvwinch(win, y, x);
			mvwaddch(win, y, x, local_c);
		}

		wattroff(win, A_REVERSE);
		pop_cursor(win);			
	};

	int get_y_of_contents(int contents_position)
	{
		if(contents_position<0)
			return 0;
		int temp_y = vertical_offset + (contents_position / (print_width));
		if(temp_y >= h)		//limits to page
			temp_y = h -1;
		return temp_y;
	};

	int get_x_of_contents(int contents_position)
	{
		if(contents_position<0)
			return 0;
		int temp_x = horizontal_offset + (contents_position % (print_width));
		return temp_x;
	};

	void push_cursor(WINDOW* win)
	{
		getyx(win, temporary_cursor_y, temporary_cursor_x);
	};

	void pop_cursor(WINDOW* win)
	{
		wmove(win, temporary_cursor_y, temporary_cursor_x);
	};

	string get_selection()
	{
		string t;
		if(!selecting)
			return t;
		else
		{
			string& c = focal_node->get_contents();
			return c.substr(selection_start(),selection_end()-selection_start());
		}
	};

	int selection_start()
	{
		int start = contents_pos;
		int end = mark_pos;
		if(start > end)
		{
			int t = start;
			start = end;
			end = t;
		}
		return start;
	};

	int selection_end()
	{
		int start = contents_pos;
		int end = mark_pos;
		if(start > end)
		{
			int t = start;
			start = end;
			end = t;
		}
		return end;
	};
};

#endif