#ifndef NODE_ITERATORS_HPP
#define NODE_ITERATORS_HPP

class node_iterator
{
protected:
	vector<node*> writing_history;
public:
	virtual node*	next_node() =0;
	node_iterator(){};
	node_iterator(vector<node*>& writing_history) : writing_history(writing_history){};
	vector<node*> get_writing_history(){return writing_history;};
};


class single_category_iterator : public node_iterator
{
	node*	start;
	bool	started = false;
	bool	reverse;
public:
	single_category_iterator(node* start, vector<node*> writing_history) : node_iterator(writing_history), start(start), reverse(false) {};
	single_category_iterator(node* start) : start(start), reverse(false){};
	single_category_iterator(node* start, bool reverse) : start(start), reverse(reverse) {};
	
	void reverse_iteration(){reverse = true;};
	node* next_node() override
	{
		if(!started)
		{
			started = true;
			return start;
		}
		if(start==NULL)
			return NULL;
		if(!reverse)
			start = start->get_successor();
		else
			start = start->get_predecessor();
		return start;
	};
};


class category_ordered_link_iterator : public node_iterator
{
	vector<node_container*>&	node_containers;
	node*						base_node;
	vector<node*>&				first_nodes;
	vector<directed_link*>&		links;
	node*						start = NULL;
	bool reached_start=true;
	bool reverse = false;
	int i;
	int j;
	int depth=1073741824; //2^30


public:
	category_ordered_link_iterator(database& db, node* base_node, vector<node*>	writing_history, int d)
	:	node_iterator(writing_history),
		node_containers(db.get_node_containers()),
		first_nodes(node_containers[0]->get_nodes()),
		base_node(base_node),
		links(base_node->get_links()),
		depth(d),
		i(0), j(0){};

	node* get_parent()
	{
		return base_node;
	};

	void set_starting_node(node* s)
	{
		start = s;
		reached_start = false;
	};

	void reverse_iteration()
	{
		reverse = true;
		i=node_containers.size() -1;
		j=links.size() -1;
	};

	directed_link*	next_link()
	{
		for(; i_condition() ; next_i())
		{
			if(node_containers[i]->get_node_type().is_enabled())
			{
				for(; j_condition(); next_j())
				{
					directed_link*	link	= links[j];
					node*			subnode = link->get_end_node();
					if(		subnode->get_node_type() == node_containers[i]->get_node_type()
						&&	!excluded(subnode))
					{

						if(subnode == start)
							reached_start = true;
						if(reached_start)
						{
							next_j();
							return link;
						}							
					}
				}
				first_j(); //not clear if needed..
			}
		}
		return NULL;
	};

	bool i_condition()
	{
		if(!reverse)
			return i<node_containers.size();
		return i>=0;
	};

	bool j_condition()
	{
		if(!reverse)
			return j<links.size();
		return j>=0;
	}

	void next_i()
	{
		if(!reverse)
			i++;
		else
			i--;
	}

	void next_j()
	{
		if(!reverse)
			j++;
		else
			j--;
	}

	void first_j()
	{
		if(!reverse)
			j=0;
		else
			j=links.size() -1;
	}
	
	node* next_node() override
	{
		directed_link* link;
		if( (link = next_link())!= NULL )
			return link->get_end_node();
		return NULL;
	};

	bool excluded(node* n)
	{
		if(writing_history.size()>= node_containers.size())
			return true;

		for(int x=0; x<node_containers.size(); x++)
		{
			if(node_containers[x]->get_node_type() == writing_history.back()->get_node_type())
			{
				for(int y=x; y<node_containers.size() && y<depth+1; y++)
				{
					if(node_containers[y]->get_node_type() == n->get_node_type())
						return false;
				}
				return true;
			}
		}
		return false;

	};
};

#endif

