
#include "gui/category_bar.hpp"
#include <string>
#include <sstream>

category_bar::category_bar(database_manager& dm) : node_containers(dm.db.get_node_containers())
{
	make_tiles();	//Needs to prepare to remake tiles in case the db changes
}

void category_bar::initialize(int termy, int termx)
{
	make(termy, termx);
}

void category_bar::make(int termy, int termx)
{
	y = 1;
	x = termx;
	starty = 0;
	startx = 0;

	if(active)
	{
		w = newwin(y, x, starty, startx);
		wtimeout(w, -1);
		draw();
	}
}

void category_bar::draw()
{
	werase(w);

	int width = x;
	int text_width = tiles.size() - 1;
	for(int i=0; i<tiles.size(); i++)
	{
		text_width += tiles[i]->length();
	}

	bool tile_shortening = false;
	if(text_width > width)
		tile_shortening = true;

	if(tiles.size()==0)
		return;

	int new_tile_size = (width - (tiles.size()-1)) / tiles.size();
	if(tile_shortening)
	{
		tiles[0]->write_on_with_size(w, new_tile_size,selecting_a_category);
		for(int i=1; i<tiles.size(); i++)
		{
			waddstr(w, "|");
			tiles[i]->write_on_with_size(w, new_tile_size,selecting_a_category);
		}	
	}
	else
	{
		int lpad = (width - text_width) /2;
		int parity = (width - text_width) % 2;
		int rpad = lpad;
		if(parity == 1)
			rpad++;
		waddstr(w, string(lpad,' ').c_str());

		tiles[0]->write_on(w,selecting_a_category);
		for(int i=1; i<tiles.size(); i++)
		{
			waddstr(w, "|");
			tiles[i]->write_on(w,selecting_a_category);
		}

		waddstr(w, string(rpad,' ').c_str());
	}

	wrefresh(w);
}

void category_bar::refresh_data()
{
	make_tiles();
	// make(1,x);
}

void category_bar::make_tiles()
{
	clear_tiles();
	int n = node_containers.size();
	for(int i=0; i<n; i++)
		tiles.push_back(new category_tile(node_containers[i]->get_node_type()));

	if(focal_tile_index!=-1)
	{
		focal_tile_index = 0;
		highlight_focal_tile();
	}

	focal_tile_index_cached = 0;
}

void category_bar::clear_tiles()
{
	for(int i=0; i<tiles.size(); i++)
		delete (tiles[i]);
	tiles.clear();
}


void category_bar::start_editing_new_category(string cat_name)
{
	for(int i=0; i<tiles.size(); i++)
	{
		if(tiles[i]->nt.get_name() == cat_name)
			give_focus_to(i);
	}
	start_editing_current_tile();
}

string category_bar::get_deleting_category_name()
{
	return tiles[focal_tile_index]->nt.get_name();
}



void category_bar::give_focus_to(int new_index)
{
	if(!(focal_tile_index== -1))
		tiles[focal_tile_index]->lose_focus();
	focal_tile_index = new_index;
	tiles[focal_tile_index]->get_focus();
	draw();
}

void category_bar::focus_move_left()
{
	int n=tiles.size();
	int i=focal_tile_index;
	int j=i-1;
	if(j<0)
		j=0;
	if(j<=get_parent_index() && selecting_a_category)
		return;
	give_focus_to(j);
}

void category_bar::focus_move_right()
{
	int n=tiles.size();
	int i=focal_tile_index;
	int j=(i+1);
	if(j==n)
		j=n-1;
	if(j>depth_limit && selecting_a_category)
		return;
	give_focus_to(j);
}

void category_bar::swap_left()
{
	int n=tiles.size();
	int i=focal_tile_index;
	int j=i-1;
	if(j<0)
		j=0;

	if(j<=get_parent_index() && selecting_a_category)
		return;

	if(!selecting_a_category)
	{
		swap_tiles(i,j);
		swap_containers(i,j);
	}
		give_focus_to(j);
}

void category_bar::swap_right()
{
	int n=tiles.size();
	int i=focal_tile_index;
	int j=i+1;
	if(j==n)
		j=n-1;
	if(!selecting_a_category)
	{
		swap_tiles(i,j);
		swap_containers(i,j);
	}
	if(j>depth_limit && selecting_a_category)
		return;
	give_focus_to(j);
}

void category_bar::swap_current_to_start()
{
	int k=focal_tile_index;
	for(int i=0; i<k; i++)
		swap_left();	
}

int category_bar::get_parent_index()
{
	int index_of_parent = -1;
	for(int i=0; i<tiles.size();i++)
	{
		if(tiles[i]->nt.get_name() == parent_name)
			index_of_parent = i;
	}
	return index_of_parent;
}

void category_bar::highlight_focal_tile()
{
	if(focal_tile_index_cached != -1)
		give_focus_to(focal_tile_index_cached);
}

void category_bar::cache_focal_tile()
{
	tiles[focal_tile_index]->lose_focus();
	focal_tile_index_cached = focal_tile_index;
	focal_tile_index = -1;
}

void category_bar::toggle_color()
{
	tiles[focal_tile_index]->toggle_color();
	draw();
}

void category_bar::toggle_enable_current_tile()
{
	tiles[focal_tile_index]->toggle_enabled();
	draw();
}


void category_bar::start_editing_current_tile()
{
	category_tile& f = *(tiles[focal_tile_index]);
	node_type& nt = f.nt;
	f.is_being_edited = true;	//don't do this if the categories are being shortened?
	draw();
	int ch = wgetch(w);
	while(ch != 10 && ch != 13)
	{
		if(ch == 27)
			break;

		string cat_name = nt.get_name();
		if(is_alphanumeric(ch))
		{
			string addend(1,ch);
			if(cat_name == " ")
				nt.set_name(addend);	
			else
				nt.set_name( (cat_name.substr(0, cat_name.length())) + addend);
			draw();
		}
		else
		if(ch == 127 && cat_name.length()>0)
		{
			string temp = cat_name.substr(0, cat_name.length()-1);
			nt.set_name(temp);
			draw();
		}
		ch = wgetch(w);
	}
	f.is_being_edited = false;
}

void category_bar::start_new_node_category_selection(int d, node* parent)
{
	parent_name = parent->get_node_type().get_name();
	depth_limit = d;
	selecting_a_category = true;

	if(!(focal_tile_index_cached>get_parent_index() && focal_tile_index_cached<=depth_limit))
		focal_tile_index_cached = get_parent_index()+1;
}

void category_bar::stop_new_node_category_selection()
{
	selecting_a_category = false;
}

bool category_bar::is_selecting_new_node_category()
{
	return selecting_a_category;
}




