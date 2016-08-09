#ifndef WINDOW_LIST_HPP
#define WINDOW_LIST_HPP

#include "gui/category_bar.hpp"
#include "gui/main_text.hpp"
#include "gui/status_bar.hpp"
#include "gui/text_window.hpp"
#include "data/database_manager.hpp"
#include "data/data_structures.hpp"
using namespace std;

class window_manager
{
	vector<text_window*> tws;
	text_window* focal_text_window = &mt;

public:
	database_manager&	dm;
	category_bar		cb;
	status_bar			sb;
	main_text			mt;

	window_manager(database_manager& dm)
	:
		dm(dm),
		cb(dm),
		mt(cb, sb, dm)
	{
		tws.push_back(&cb);
		tws.push_back(&mt);
		tws.push_back(&sb);
	};

	// rendering
	void initialize(int y, int x){resize(y,x);};
	void resize(int y, int x)
	{
		for(int i=0; i<tws.size(); i++)
			tws[i]->resize(y,x);
		redraw();
	};
	void redraw()
	{
			for(int i=0; i<tws.size(); i++)
		tws[i]->draw();
	};
	// void toggle_category_bar(bool active){cb.set_activity(active);};
	void refresh_data() {cb.refresh_data();	mt.refresh_data();};

	// focus
	void update_focus(text_window* new_focus)
	{
		focal_text_window->lose_focus();
		focal_text_window = new_focus;
		focal_text_window->get_focus();
		redraw();
	};
	WINDOW* get_focal_window(){ return focal_text_window->window();};
	
	// input handling
	void enter(){
		if(focal_text_window == &mt)
			mt.enter();
		if(focal_text_window == &cb)
		{
			if(cb.is_selecting_new_node_category())
			{
				string cat_name = cb.get_focal_type().get_name();
				// cb.pop_focal_tile();
				cb.cache_focal_tile();
				cb.stop_new_node_category_selection();
				update_focus(&mt);
				mt.tv.make_new_node_for_editing(cat_name);
			}
			else
				cb.start_editing_current_tile();
		}
	};
	void alphanumeric(int ch)
	{
		if(focal_text_window == &cb)
		{
			if(ch == 'c')
				cb.toggle_color();		//rename, to pass along a pure keystroke to the cb?
			if(ch == ' ')
				cb.toggle_enable_current_tile();
			if(ch == 'n')
			{	//New node

				node_type nt = cb.get_focal_type();
				if(dm.get_category(nt.get_name())!=NULL)
				{
					node* n= mt.tv.get_current_selection();
					if( !(n->get_node_type() == nt) )
					{
						cb.swap_current_to_start();
						// refresh_data();
					}
					cb.cache_focal_tile();
					update_focus(&mt);
					mt.tv.make_new_node_for_editing(nt.get_name());
				}
			}
			if(ch == 'a')
			{	//Add category
				dm.add_category(" ");
				cb.refresh_data();	//just cb?
				cb.start_editing_new_category(" ");
			}
		}
		else
		if(focal_text_window == &mt)
		{
			if(!mt.tv.ne.is_editing() && ch== 'n')
			{
				if(!cb.is_selecting_new_node_category())
				{
					cb.start_new_node_category_selection(mt.tv.get_depth_limit(),mt.tv.get_focal_parent());
					// cb.push_focal_tile();
					update_focus(&cb);
					cb.highlight_focal_tile();
				}
			}
			else
				mt.alphanumeric(ch);
		}
	};
	void large_up()
	{
		if(focal_text_window == &mt && mt.tv.ne.is_editing())
			return;
		if( (focal_text_window == &mt) && cb.is_active())
		{
			update_focus(&cb);
			cb.highlight_focal_tile();
		}
	};
	void large_down()
	{
		if(focal_text_window == &cb)
		{
			cb.cache_focal_tile();
			update_focus(&mt);
		}
	};
	void large_left(){};
	void large_right(){};
	void up(){ if(focal_text_window == &mt) mt.up();};
	void down(){ if(focal_text_window == &mt) mt.down();};
	void left()
	{
		if(focal_text_window == &cb)
			cb.focus_move_left();
		if(focal_text_window == &mt)
			mt.left();
	};
	void right()
	{
		if(focal_text_window == &cb)
			cb.focus_move_right();
		if(focal_text_window == &mt)
			mt.right();
	};
	void shift_up(){if(focal_text_window == &mt) mt.shift_up();};
	void shift_down(){if(focal_text_window == &mt) mt.shift_down();};
	void shift_left()
	{
		if(focal_text_window == &cb)
			cb.swap_left();
		if(focal_text_window == &mt)
			mt.shift_left();
	};
	void shift_right()
	{
		if(focal_text_window == &cb)
			cb.swap_right();	
		if(focal_text_window == &mt)
			mt.shift_right();
	};
	void ctrl_v(){if(focal_text_window == &mt) mt.ctrl_v();};
	void ctrl_c(){if(focal_text_window == &mt) mt.ctrl_c();};
	void home(){if(focal_text_window == &mt) mt.home();};
	void end(){if(focal_text_window == &mt) mt.end();};
	void ctrl_h(){if(focal_text_window == &mt) mt.ctrl_h();};
	void handle_delete()
	{
		if(focal_text_window == &cb)
		{
			if(dm.db.get_node_containers().size() >1)
			{
				dm.delete_category(cb.get_deleting_category_name());
				refresh_data();
			}
		}
		else
		if(focal_text_window == &mt)
		{
			if(mt.handle_delete())
				refresh_data();
		}
		redraw();
	};
	void handle_backspace(){if(focal_text_window == &mt) mt.backspace();};
	void set_status(string s){sb.set_status(s);};

	void handle_cancel()
	{
		if(cb.is_selecting_new_node_category())
		{
			cb.cache_focal_tile();
			cb.stop_new_node_category_selection();
			update_focus(&mt);	
		}	
	};
};

#endif