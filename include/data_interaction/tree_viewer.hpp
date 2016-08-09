#ifndef TREE_VIEWER_HPP
#define TREE_VIEWER_HPP

#include "gui/status_bar.hpp"
#include "data/database_manager.hpp"
#include "data/data_structures.hpp"
#include <ncurses.h>
#include "data_interaction/node_iterators.hpp"
#include "data_interaction/node_editor.hpp"
#include "data_interaction/node_writer.hpp"
#include <vector>

class tree_viewer
{
	database_manager& dm;
	database&		db;
	bool			has_focus = true;

	status_bar&		sb;
	vector<node*>	selection_history;	
	int				depth_limit = 1;

	bool linking = false;
	node* linkee;
	int linkos_number = 0;

	WINDOW* win;
	int w;
	int h;

	bool had_to_scroll = false;
	bool reached_selection = false;

public:
	node_editor		ne;
	node_writer		nw;
	tree_viewer(database_manager& dm, status_bar& sb)
	: dm(dm), db(dm.db), sb(sb), nw() {refresh_data();};

	bool is_linking() { return linking;};
	int get_depth_limit() {return depth_limit;};
	void get_focus() {has_focus = true;};
	void lose_focus() {has_focus = false;};

//display
	void write_on(WINDOW* win, int h, int w);
	// graph-walking
	bool write_nodes_and_links_from_iterator(single_category_iterator& i, vector<node*> writing_history);
	bool write_nodes_and_links_from_link_iterator(category_ordered_link_iterator& i, vector<node*> writing_history);
	bool write_links_of(node* n, vector<node*> writing_history);
	// single-item
	bool write_node(node* n, vector<node*> writing_history);
	bool write_link(int max_relation_length, directed_link* dl, vector<node*> writing_history);	
	// single-item wrap-up
	bool write_reflection(node* n, vector<node*> writing_history);
	bool reached_selection_according_to(node* n, vector<node*> wh);

//navigation and response to input
	void up();
	void down();
	void right();
	void left();
	void handle_internal_traversal(string direction);
	void handle_traversal(string direction);
	void enter();
	void shift_left();
	void shift_right();
	void shift_up();
	void shift_down();
	void handle_swap(string direction);
	bool handle_delete();

	void home(){if(ne.is_editing()) ne.home();};
	void end(){if(ne.is_editing()) ne.end();};
	void backspace(){if(ne.is_editing()) ne.backspace();};
	void alphanumeric(int ch);

//editing and linking
	void open_for_editing();
	void make_new_node_for_editing(string cat);
	void make_new_node_for_editing();
	void ctrl_v()
	{
		if(ne.is_editing()) ne.paste();
	};
	void ctrl_c()
	{
		if(ne.is_editing()) ne.clip();
	};
	void stop_linking()
	{
		linking = false;
		linkos_number = 0;
		sb.stop_linking();
	}

//auxiliary
	bool check_first_node_selection();

	void initialize_environment_variables(WINDOW* window, int height, int width)
	{
		h = height;
		w = width;
		win = window;
		ne.initialize(window, height, width);
		nw.initialize(window, height, width);
		curs_set(0);
		scrollok(win,true);
	};

	void refresh_data()
	{
		selection_history.clear();
		check_first_node_selection();
		ne.refresh_data();
	};

	bool is_alphanumeric (int ch){ return (32 <= ch && ch <= 126); };
	
	bool is_alphanumeric (string s){
		for(int i=0; i<s.length(); i++)
			if(!is_alphanumeric(s[i]))
				return false;
			return true;
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

	void ctrl_h(){};

	bool is_on_first_category()
	{
		return selection_history.size() == 1;
	};

	node* get_current_selection()
	{
		if(selection_history.size() > 0)
			return selection_history.back();
		else
			return NULL;
	};

	void hard_reset_selection(node* n)
	{
		selection_history.back()->lose_focus();
		selection_history.clear();
		selection_history.push_back(n);
		n->get_focus();
	};

	void set_selection(node* n)
	{
		int m = selection_history.size();
		selection_history[m-1]->lose_focus();
		selection_history.erase(selection_history.begin() + (m-1));
		selection_history.push_back(n);
		n->get_focus();
	};

	node* get_focal_parent()
	{
		int n = selection_history.size();
		if(n<2)
			return NULL;
		else
			return selection_history[n-2];
	};
};
#endif

