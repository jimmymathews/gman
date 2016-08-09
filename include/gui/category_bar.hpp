#ifndef CATEGORY_BAR_HPP
#define CATEGORY_BAR_HPP

#include "data/database_manager.hpp"
#include "data/data_structures.hpp"
#include "gui/category_tile.hpp"
#include "gui/text_window.hpp"
#include <ncurses.h>
#include <vector>
using namespace std;

class category_bar : public text_window
{
	vector<node_container*>&	node_containers;
	vector<category_tile*>		tiles;
	int	focal_tile_index = -1;
	int depth_limit = 1;
	string parent_name="";
	int focal_tile_index_cached = 0;
	bool selecting_a_category = false;

public:
	category_bar(database_manager& db);
	void initialize(int termy, int termx) override;
	void make(int termy, int termx) override;
	void draw() override;
	void refresh_data();

	node_type get_focal_type()
	{
		if(focal_tile_index != -1)
			return tiles[focal_tile_index]->nt;
		node_type dummy;
		return dummy;
	};

	void start_editing_new_category(string cat_name);
	void give_focus_to(int new_index);
	void focus_move_left();
	void focus_move_right();
	string get_deleting_category_name();
	void toggle_color();
	void toggle_enable_current_tile();
	void start_editing_current_tile();
	bool is_alphanumeric(int ch) { return (32 <= ch && ch <= 126); };
	void swap_right();
	void swap_left();
	void swap_current_to_start();
	void highlight_focal_tile();
	void cache_focal_tile();
	// void push_focal_tile();
	// void pop_focal_tile();
	void make_tiles();
	void clear_tiles();
	void swap_containers(int i, int j)
		{	node_container* nc1 = node_containers[i];
			node_container* nc2 = node_containers[j];
			node_containers[i] = nc2;
			node_containers[j] = nc1;};
	void swap_tiles(int i, int j)
		{	category_tile* ct1 = tiles[i];
			category_tile* ct2 = tiles[j];
			tiles[i] = ct2;
			tiles[j] = ct1;};
	void start_new_node_category_selection(int d, node* parent);
	void stop_new_node_category_selection();
	bool is_selecting_new_node_category();
	int get_parent_index();
	~category_bar(){clear_tiles();};
};

#endif