#include "data_interaction/tree_viewer.hpp"

void tree_viewer::write_on(WINDOW* window, int height, int width)
{
	initialize_environment_variables(window, height, width);

	if(db.there_is_no_data() || !check_first_node_selection())
		return;

	had_to_scroll = false;
	reached_selection = false;

	node* head = db.get_node_containers()[0]->get_head();
	single_category_iterator i(head);
	vector<node*> writing_history;
	write_nodes_and_links_from_iterator(i, writing_history);
}


//Recursive graph-walking logic

bool tree_viewer::write_nodes_and_links_from_iterator(single_category_iterator& i, vector<node*> writing_history)
{
	node* subnode;
	while( (subnode=i.next_node()) != NULL)
	{
		if(!write_node(subnode, writing_history))
			return false;		
		vector<node*> local_copy_of_writing_history = writing_history;
		local_copy_of_writing_history.push_back(subnode);
		if(!write_links_of(subnode,local_copy_of_writing_history))
			return false;
	}
	return true;
}

bool tree_viewer::write_nodes_and_links_from_link_iterator(category_ordered_link_iterator& i, vector<node*> writing_history)
{
	directed_link* dl;
	int max_relation_length = i.calculate_max_relation_length();
	while( (dl=i.next_link()) != NULL)
	{
		node* subnode = dl->get_end_node();
		if(!write_link(max_relation_length,dl, writing_history))
			return false;
		vector<node*> local_copy_of_writing_history = writing_history;
		local_copy_of_writing_history.push_back(subnode);
		if(!write_links_of(subnode,local_copy_of_writing_history))
			return false;
	}
	return true;
}

bool tree_viewer::write_links_of(node* n, vector<node*> writing_history)
{
	if(writing_history.size()>depth_limit)
		return true;
	category_ordered_link_iterator i(db,n,writing_history,depth_limit);
	return write_nodes_and_links_from_link_iterator(i, writing_history);
}


//Single item
bool tree_viewer::write_node(node* n, vector<node*> writing_history)
{
	if(n->has_focus() && ne.is_editing_specific_element(writing_history))
		ne.write_editing_node(n);
	else
		ne.nw.write_to_end_of_line(n, writing_history.size());
	return write_reflection(n,writing_history);
}

bool tree_viewer::write_link(int max_relation_length, directed_link* dl, vector<node*> writing_history)
{
	node* n = dl->get_end_node();
	if(n->has_focus() && ne.is_editing_specific_element(writing_history))
		ne.write_editing_node(n);
	else
		ne.nw.write_link(max_relation_length, dl, writing_history.size());
	return write_reflection(n,writing_history);
}

//Wrap up for single item
bool tree_viewer::write_reflection(node* n, vector<node*> writing_history)
{
	if(!reached_selection)
		if(reached_selection_according_to(n,writing_history))
			reached_selection = true;
	if(cursor_y() == h-1)
		had_to_scroll = true;		
	if(had_to_scroll && reached_selection)
		return false;
	return true;
}

bool tree_viewer::reached_selection_according_to(node* n, vector<node*> wh)
{
	wh.push_back(n);
	
	if(selection_history.size() == wh.size())
	{
		for(int i=0; i<wh.size();i++)
			if(wh[i] != selection_history[i])
				return false;
		return true;
	}
	else
		return false;
}






void tree_viewer::handle_traversal(string direction)
{
	node* cached_selection = get_current_selection();

	handle_internal_traversal(direction);

	node* new_selection = get_current_selection();
	if(cached_selection != new_selection)
	{
		if(cached_selection!=NULL)
			cached_selection->lose_focus();
		if(new_selection!=NULL)
			new_selection->get_focus();
		if(cached_selection == NULL || new_selection == NULL)
			sb.set_temporary_status("Focus error");
	}
}

void tree_viewer::handle_internal_traversal(string direction)
{
	int n = selection_history.size();

	if(n==0)
	{
		sb.set_temporary_status("error: selection_history empty");			//debug
		return;																//make sure n is never 0
	}

	if(direction == "left")
	{
		if(n==1) return;
		if(n>1)
			selection_history.erase(selection_history.begin()+ (n-1) );
	}

	if(direction == "right")
	{
		category_ordered_link_iterator i(db, selection_history.back(), selection_history,depth_limit);
		node* next = i.next_node();

		if(next !=NULL && selection_history.size()< 1+depth_limit)
			selection_history.push_back(next);
		else
		{
			//fake down
			if(n>=2)
			{
				category_ordered_link_iterator id(db, selection_history[n-2], selection_history,depth_limit);
				id.set_starting_node( selection_history[n-1] );
				id.next_node();
				next = id.next_node();
				if(next == NULL)
					return;
				else
				{
					selection_history.erase(selection_history.begin()+(n-1));
					selection_history.push_back(next);
				}
			}
		}
	}

	if(direction == "up" || direction == "down")
	{
		node* new_node = NULL;
		if(n == 1)
		{
			single_category_iterator i(selection_history.back());	
			if(direction == "up")
				i.reverse_iteration();
			node* next = i.next_node();
			if(! ((next=i.next_node())==NULL) )
			{
				new_node = next;
			}
		}

		if(n >= 2)
		{
			vector<node*> popped;
			for(int i=0; i<selection_history.size()-1; i++)
				popped.push_back(selection_history[i]);			//dangling
			
			category_ordered_link_iterator i(db, selection_history[n-2], popped,depth_limit);
			i.set_starting_node( selection_history[n-1] );
			if(direction == "up")
				i.reverse_iteration();

			node* next = i.next_node();
			if( (next = i.next_node()) != NULL)
			{
				new_node = next;
			}
		}

		if(new_node != NULL)
		{
			selection_history.erase(selection_history.begin()+ n-1);
			selection_history.push_back(new_node);
		}
		else
		{
			if(direction == "down")
				handle_internal_traversal("right");
			if(direction == "up")
				handle_internal_traversal("left");
		}
	}
}

void tree_viewer::left()
{
	if(!ne.is_editing()) handle_traversal("left");
	else ne.navigation_action("left");
}

void tree_viewer::right()
{
	if(!ne.is_editing()) handle_traversal("right");
	else ne.navigation_action("right");
}

void tree_viewer::up()
{
	if(!ne.is_editing()) handle_traversal("up");
	else ne.navigation_action("up");
}

void tree_viewer::down()
{
	if(!ne.is_editing()) handle_traversal("down");
	else ne.navigation_action("down");
}

void tree_viewer::shift_left()
{
	if(ne.is_editing())
		ne.shift_navigation_action("left");
	else
	if(depth_limit>selection_history.size()-1)
		if(!(selection_history.back()->get_node_type() == db.get_node_containers()[depth_limit]->get_node_type()))
			depth_limit--;
}

void tree_viewer::shift_right()
{
	if(ne.is_editing())
		ne.shift_navigation_action("right");
	else
	{
		depth_limit++;
		if(depth_limit >= db.get_node_containers().size())
			depth_limit = db.get_node_containers().size()-1;
	}
}

void tree_viewer::shift_up()
{
	if(ne.is_editing())
		ne.shift_navigation_action("up");
	else
		handle_swap("up");
}

void tree_viewer::shift_down()
{
	if(ne.is_editing())
		ne.shift_navigation_action("down");
	else
		handle_swap("down");
}

void tree_viewer::handle_swap(string direction)
{
	node* cached = get_current_selection();
	if(cached == NULL)	
		return;

	int n = selection_history.size();
	if(n>1)
	{
		node* parent = selection_history[n-2];
		vector<directed_link*>& children = parent->get_links();

		vector<node*> popped;
		for(int i=0; i<selection_history.size()-1; i++)
			popped.push_back(selection_history[i]);
		
		category_ordered_link_iterator i(db, parent, popped,depth_limit);
		i.set_starting_node(selection_history[n-1]);
		if(direction == "up")
			i.reverse_iteration();
		directed_link* start = i.next_link();
		directed_link* next = i.next_link();
		if(next == NULL || start == NULL)
			return;
		if( start->get_end_node() != selection_history[n-1] )
			return;
		if(start->get_end_node()->get_node_type() == next->get_end_node()->get_node_type())
		{
			parent->swap_links(start,next);
		}
		return;
	}

	handle_internal_traversal(direction);

	node* new_selection = get_current_selection();
	if(new_selection == NULL) //
		return;
	
	if(cached->get_node_type() == new_selection->get_node_type())
	{
		dm.swap_nodes(cached,new_selection);
		int n = selection_history.size();
		selection_history.erase(selection_history.begin() + (n-1) );
		selection_history.push_back(cached);
	}
	else
	{
		cached->lose_focus();
		new_selection->get_focus();
	}
}

void tree_viewer::enter()
{
	if(linking)
	{
		stop_linking();
		return;
	}

	if(ne.is_editing())
	{
		if(	ne.stop_editing() )
			handle_delete();
	}
	else
		ne.start_editing(selection_history);
}

bool tree_viewer::handle_delete()
{
	if(ne.is_editing())
		return ne.handle_delete();
	
	int n = selection_history.size();	
	if(n == 1)
	{
		node* cached = selection_history.back();
		node* new_node = cached->get_successor();
		if(new_node == NULL)
		{
			new_node = cached->get_predecessor();
			if(new_node == NULL)
			{
				dm.delete_category(cached->get_node_type().get_name());
				return true;
			}
		}
		
		selection_history.erase(selection_history.begin() + (n-1));
		selection_history.push_back(new_node);
		dm.get_category(cached->get_node_type().get_name())->delete_node(cached);
	}
	else
	{
		node* cached = selection_history.back();
		node_container* cached_category = dm.get_category(cached->get_node_type().get_name());
		handle_traversal("down");
		if(cached == selection_history.back())
		{
			handle_traversal("up");
			if(cached == selection_history.back())
			{
				handle_traversal("left");
				if(cached == selection_history.back())  // assert selection_history.back() != cached ?
					return false;
				if(cached_category->get_nodes().size()==1)
				{
					dm.delete_category(cached_category->get_node_type().get_name());
					return true;	//triggers data refresh
				}
			}
		}
		cached_category->delete_node(cached);  //data refresh supposedly not needed
	}
	return false;
}

void tree_viewer::alphanumeric(int ch)
{
	if(ne.is_editing())
		ne.alphanumeric(ch);
	else
	{
		if(ch == 'n')
			make_new_node_for_editing();
		if(ch == 'l' && linking)
		{
			stop_linking();
		} 
		if(ch == 'l' && !linking)
		{
			linking = true;
			linkee = selection_history.back();
			sb.use_link_dialog(selection_history.back());
		}
		if(ch == 'u')
		{
			int n = selection_history.size();
			if(n>=2)
			{
				node* parent = selection_history[n-2];
				node* child = selection_history[n-1];
				handle_traversal("up");
				child->unlink(parent);
			}
		}
		if(ch == 'p')
		{
			//Prune tree at given node; cut once, then get subgraph connected to this node?
		}
		if(ch == 't' && linking)
		{
			linkee->link(selection_history.back());
			linkos_number++;
			sb.notify_of_new_linko(selection_history.back(),linkos_number);
		}
	}
}


//editing
void tree_viewer::open_for_editing()
{
	enter();
}

void tree_viewer::make_new_node_for_editing(string cat)
{
	int n = selection_history.size();
	
	if(n == 1)
	{
		node* added = dm.add_node("",cat,true);
		set_selection(added);
		open_for_editing();
	}
	else
	if (n > 1)
	{
		node* added = dm.add_node("",cat,true);
		node* parent = selection_history[n-2];
		added->link(parent);
		set_selection(added);
		open_for_editing();
	}
}

void tree_viewer::make_new_node_for_editing()
{
	string cat = selection_history.back()->get_node_type().get_name();
	make_new_node_for_editing(cat);
}



//auxiliary
bool tree_viewer::check_first_node_selection()
{
	if(db.get_node_containers().size() ==0)
		return false;
	
	node_container*	first_category	= db.get_node_containers()[0];

	node* first_category_head = first_category->get_head();
	if( first_category_head == NULL)
		return false;

	if(selection_history.size() == 0)
	{
		selection_history.push_back(first_category_head);
		first_category_head->get_focus();
	}
	else
	if(!(selection_history[0]->get_node_type() == first_category_head->get_node_type()))
	{
		selection_history.back()->lose_focus();
		selection_history.clear();
		selection_history.push_back(first_category_head);
		first_category_head->get_focus();
	}
	return true;
};