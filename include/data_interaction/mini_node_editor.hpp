#ifndef MINI_NODE_EDITOR_HPP
#define MINI_NODE_EDITOR_HPP

#include "config_p.hpp"
#include "node_writer.hpp"
#include "big_node_editor.hpp"

class mini_node_editor
{
	//State
	WINDOW* win;
	int h;
	int w;
	bool editing = false;
	bool selecting = false;
	bool using_big_editor = false;
	bool greek_input = false;
	bool bout_to_superscript = false;
	bool bout_to_subscript = false;
	node* focal_node = NULL;
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
	node_writer nw;
	big_node_editor bne;
	mini_node_editor() : bne(nw)
	{
		focal_selection_history.push_back(NULL);
	};

	//State
	void initialize(WINDOW* window, int height, int width)
	{
		win = window;
		h = height;
		w = width;
		nw.initialize(window, height, width);
	};

	void refresh_data()
	{
		stop_editing();
	};

	void toggle_greek_mode()
	{
		greek_input = !greek_input;
	};

	void enter()
	{
		if(using_big_editor)
		{
			alphanumeric(10);		
			// bne.enter...
		}

	};

	void use_big_editor()
	{
		using_big_editor = true;
		print_width = w;
	};

	void turn_off_big_editor()
	{
		using_big_editor = true;
		int depth = focal_selection_history.size()-1;
		if(depth >=0)
			print_width = w - (depth*config_p::tab_size);
	};

	void start_editing(vector<node*> fsh)
	{
		int n = fsh.size();
		if(n == 0)
			return;
		focal_selection_history = fsh; //?
		int depth = fsh.size()-1;
		print_width = w - (depth*config_p::tab_size);
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

		//test
		// use_big_editor();
	};

	bool stop_editing()
	{
		editing = false;
		selecting = false;
		using_big_editor = false;
		contents_pos = 0;
		mark_pos = -1;
		if(focal_selection_history.back() == NULL)
			return false;

		string combo = focal_selection_history.back()->get_contents();
		string& contents_reference = focal_selection_history.back()->get_contents();
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
				dl->set_name(get_new_relation_descriptor(combo));
		}
		
		if(focal_selection_history.back()->get_contents() == "")
			return true;
		return false;
	};

	bool is_editing() { return editing;};
	bool is_using_big_editor() { return using_big_editor;};

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
		int x;
		int y;
		getyx(win, y,x);

		vertical_offset = y;
		int depth = focal_selection_history.size() -1;
		horizontal_offset = config_p::tab_size*depth;

		if(!using_big_editor)
			nw.write_editing_node(n, vertical_offset, horizontal_offset, print_width, selection_start(), selection_end(), selecting);
		else
			bne.write(n,selection_start(), selection_end(), selecting);
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
		string& contents_reference = focal_selection_history.back()->get_contents();
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
			if(contents_pos == 0)
				return;
			int position_of_newline;
			if( (position_of_newline = last_newline_between(contents_pos-print_width, contents_pos-1)) !=-1 )
				contents_pos = min(position_of_newline+1,contents_size-1);
			else
			{
				if(contents_pos >= print_width)
				{
					contents_pos = contents_pos - print_width;
				}
				else
					contents_pos = 0;
			}
		}
		if(direction == "down")
		{
			if(contents_pos >= contents_size)
				return;
			if(contents_reference.at(contents_pos) == '\n')
			{
				contents_pos = min (contents_pos + 1, contents_size -1);
				return;
			}
			int position_of_newline;
			if( (position_of_newline = first_newline_between(contents_pos+1, contents_pos+print_width)) !=-1 )
				contents_pos = min(position_of_newline+1,contents_size-1);
			else
			if(contents_pos + print_width < contents_size+1)
				contents_pos = contents_pos + print_width;
		}
	};

	int first_newline_between(int a, int b)
	{
		string& contents_reference = focal_selection_history.back()->get_contents();
		int n = contents_reference.size();
		string bit = contents_reference.substr(a,min(n,b)-a);

		size_t p = bit.find('\n');
		if(p == string::npos)
			return -1;
		else
			return (static_cast<int>(p)) + a;
	};

	int last_newline_between(int a, int b)
	{
		string& contents_reference = focal_selection_history.back()->get_contents();
		string bit = contents_reference.substr(max(a,0),b-max(a,0));

		size_t p = bit.rfind('\n');
		if(p == string::npos)
			return -1;
		else
			return (static_cast<int>(p)) + max(a,0);
	};

	void home()
	{
		string& contents_reference = focal_selection_history.back()->get_contents();
		if(contents_pos == 0)
			return;
		if(contents_reference.at(contents_pos-1) == '\n')
			return;

		basic_action("up");
		// string& contents_reference = focal_selection_history.back()->get_contents();
		// int n = contents_reference.size();
		// contents_pos = min(n-1, contents_pos + 1);
		// int position_of_new_line =  first_new_line_between(contents_pos+1,contents_pos + print_width);
		// if(position_of_new_line != -1)
		// 	contents_pos = position_of_new_line;
		// // else
		// 	contents_pos  = (contents_pos / print_width) * print_width;

	};

	void end()
	{
		// int temp = contents_pos;
		string& contents_reference = focal_selection_history.back()->get_contents();
		int n = contents_reference.size();

		if(contents_pos >= n)
			return;

		if(contents_reference.at(contents_pos) == '\n')
			return;

		int temp = contents_pos;
		basic_action("down");
		if(temp == contents_pos)
		{
			contents_pos = contents_reference.size();
			return;
		}
		
		
		contents_pos = min(n-1, max(contents_pos - 1,0));

		// string& contents_reference = focal_selection_history.back()->get_contents();
		// int contents_size = contents_reference.length();

		// if(contents_pos>=contents_size-1)
		// 	contents_pos = contents_size;
		// else
		// {
		// 	int red = contents_pos / print_width;
		// 	contents_pos = (red+1)*print_width -1;
		// }
		// if(contents_pos>=contents_size-1)
		// 	contents_pos = contents_size;		
	};


	//Deletion
	bool handle_delete()
	{
		string& contents_reference = focal_selection_history.back()->get_contents();
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
		string& contents_reference = focal_selection_history.back()->get_contents();
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
		//	32~126: 						regular chars, includes, numerals 48~57, symbols before that, and 65~122 letters
		//	48~57 - (48-14) = 14~23:		superscript numerals
		//	48~57 + (128-48) = 128~137:		subscript numerals
		//	97~122 + (138-97) = 138~163:	subscript lowercase letters
		//	65~122 + 128 = 193~250:			greek letters
		//	24,25,27,28:					special +/- sup and superscripts	

		unsigned char char_cache = ch;
		if(greek_input)
		{
			greek_input = false;
			char_cache = ch + 128;
		}
		else
		{
			if(32<=ch && ch<=126)
				char_cache = ch;

			if(bout_to_superscript)
			{
				bout_to_superscript = false;
				if(ch >= 48 && ch <=57)
					char_cache = ch - (48-14);
				if(ch == 43)
					char_cache = 24;	//+
				if(ch == 45)
					char_cache = 25;	//-
			}

			if(bout_to_subscript)
			{
				bout_to_subscript = false;
				if( (ch>=48 && ch<=57))
					char_cache = ch + (128-48);
				if(ch=='a' || ch=='e' || ch=='o' || ch=='x' || ch=='h' ||ch =='k' || ch=='l'|| ch=='m'|| ch=='n'|| ch=='p'|| ch=='s'|| ch=='t')
					char_cache = ch + (138-97);
				if(ch == 43)
					char_cache = 27;	//+
				if(ch == 45)
					char_cache = 28;	//-
			}

			if(ch == 95)
			{
				bout_to_subscript = true;
				return;
			}
			if(ch == 94)
			{
				bout_to_superscript = true;
				return;
			}
		}

		if(ch == 10)
			char_cache = '\n';

		string str1char(1,char_cache);
		string& contents_reference = focal_selection_history.back()->get_contents();
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

		//encodes upper lower greek and superscripts as chars in these strings... not safe for ordinary string processing after this point.
		// the point is to keep the simplicity of string length / substring operations
		contents_reference.insert(start, str1char );
		contents_pos= start+1;
	};

	void clip(){ copy_to_my_clip(get_selection());};

	void copy_to_my_clip(string in){ clip_cache = in;};

	void paste()
	{
		string& contents_reference = focal_selection_history.back()->get_contents();
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
	string get_selection()
	{
		string t;
		if(!selecting)
			return t;
		else
		{
			string& c = focal_selection_history.back()->get_contents();
			return c.substr(selection_start(),selection_end()-selection_start());
		}
	};

	int selection_start()
	{
		if(!selecting)
			return contents_pos;
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