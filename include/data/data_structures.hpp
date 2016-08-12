#ifndef DATA_STRUCTURES_HPP
#define DATA_STRUCTURES_HPP

#include <ncurses.h>
#include <string>
#include <vector>
using namespace std;

class node_type
{
	string	name="";
	int		color_index	= 0;
	bool	enabled		= true;
	string  temporary_id;

public:
	void	set_id(string id)	{temporary_id = id;};
	string	get_id()			{return temporary_id;};
	string	get_name()			{return name;};
	void	set_name(string n)	{name = n;};
	int		get_color_index()	{return color_index;};
	bool	is_enabled()		{return enabled;};

	node_type(string name) : name(name){};
	node_type(){};
	void	toggle_enabled()			{enabled = !enabled;};
	bool	operator==(node_type& nt)	{return nt.get_name() == name;};

	void	set_color(int i)			{color_index = i; };
	void	turn_on_color(WINDOW* w)	{wattron(w, COLOR_PAIR(color_index));};
	void	turn_off_color(WINDOW* w)	{wattroff(w, COLOR_PAIR(color_index));};
	void	toggle_color()				{set_color((color_index+1)%8);};
};


class node;
class directed_link
{
	string	name;
	node*	end_node = NULL;

public:
	directed_link(string name, node* end_node) : name(name), end_node(end_node) {};
	node*	get_end_node()	{return end_node;};
	string	get_name()		{return name;};
	void	set_name(string n){name = n;};
};


class node
{
	bool		focus = false;
	int			identifying_substring_length = 150;
	string		content = "";
	node_type	dummy;
	node_type&	nt;
	time_t		timer;
	node*		alphabetical_predecessor = NULL;
	node*		alphabetical_successor = NULL;
	vector<directed_link*>	links; //manage alphabetical order of these?
	// int			max_relation_length=0;
	string					temporary_id;
public:
	void					lose_focus()		{focus = false;};
	void					get_focus()			{focus = true;};
	bool					has_focus()			{return focus;};
	void turn_on_formatting(WINDOW* w)
	{
		nt.turn_on_color(w);
		if(focus)
			wattron(w, A_BOLD);
	};
	void turn_off_formatting(WINDOW* w)
	{
		if(focus)
			wattroff(w, A_BOLD);
		nt.turn_off_color(w);
	};
	string					get_id()			{return temporary_id;};
	void					set_id(string id)	{temporary_id = id;};
	string					contents()			{return content;};
	string&					get_contents()		{return content;};
	node_type&				get_node_type()		{return nt;};
	node*					get_predecessor()	{return alphabetical_predecessor;};
	node*					get_successor()		{return alphabetical_successor;};
	vector<directed_link*>& get_links()			{return links;};
	node(string content) : dummy(""), content(content), nt(dummy) {};
	node(string& content, node_type& nt) : dummy(""), content(content), nt(nt) {};

	void set_predecessor(node* p){alphabetical_predecessor = p;};
	void set_successor(node* s){alphabetical_successor = s;};

	// void update_max_relation_length()
	// {
	// 	int temp = 0;
	// 	for(int i=0; i<links.size(); i++)
	// 	{
	// 		int ll = links[i]->get_name().length();
	// 		if(ll > temp)
	// 			temp = ll;
	// 	}
	// 	max_relation_length = temp;
	// };

	// int get_max_relation_length()
	// {
	// 	return max_relation_length;
	// };

	void link(string relation, string relation_reversal, node* other)
	{
		links.insert(links.begin(),new directed_link(relation,other));
		other->get_links().insert(other->get_links().begin(),new directed_link(relation_reversal,this));	//should be alphabetically managed?
		// update_max_relation_length();
	};
	void one_way_link(string relation, node* other)
	{
		links.insert(links.begin(),new directed_link(relation,other));	
		// update_max_relation_length();
	};
	void link(vector<node*> others)
	{
		for(int i=0; i<others.size(); i++)
		{
			if(!linked_to(others[i]))
				link(others[i]);
		}
	};
	void swap_links(directed_link* l1, directed_link* l2)
	{
		for(int i=0; i<links.size();i++)
		{
			if(links[i] == l1)
				links[i] = l2;
			else
			if(links[i] == l2)
				links[i] = l1;
		}
	};
	void link(node* other)					{ link("","",other);};
	void link(string relation, node* other)	{ link(relation,"",other);};
	bool linked_to(node* other)				
	{
		for(int i=0; i<links.size();i++)
		{
			if(links[i]->get_end_node() == other)
				return true;
		}
		return false;
	};


	void unlink(node* other)
	{
		vector<directed_link*>::iterator it;
  		
		for ( it = links.begin(); it != links.end(); )
		{
			if( (*it)->get_end_node() == other )
			{
				delete * it;  
				it = links.erase(it);
			}
			else {++it;}
		}
		
		vector<directed_link*>& olinks = other->get_links();
		for ( it = olinks.begin(); it != olinks.end(); )
		{
			if( (*it)->get_end_node() == this )
			{
				delete * it;  
				it = olinks.erase(it);
			}
			else {++it;}
		}
		// update_max_relation_length();
	};

	bool operator==(node& n){return (contents() == n.contents());};
	bool operator<(node& n)	{return (contents() < n.contents());};	//should this be based id's? ... f performance

	~node()
	{
		vector<directed_link*>::iterator it;
  		
		for ( it = links.begin(); it != links.end(); )
		{	//Iterate over my links
		
			//Get other's links
			vector<directed_link*>& olinks = (*it)->get_end_node()->get_links();
			vector<directed_link*>::iterator oit;
			for(oit = olinks.begin(); oit != olinks.end();)
			{
				if((*oit)->get_end_node()==this)
				{
					delete *oit; //Delete his link if it links back here
					oit = olinks.erase(oit);
				}
				else{++oit;}
			}

			delete *it;  //Always delete this link to another
			it = links.erase(it);
		}
	} 
};

class node_container
{
	node_type		nt;
	vector<node*>	nodes;
	node*			head;
	node*			tail;

public:
	node_type&		get_node_type()	{return nt;};
	vector<node*>&	get_nodes()		{return nodes;};
	node*			get_head()		{return head;};
	node*			get_tail()		{return tail;};
	void			set_head(node* n){head = n;};
	void			set_tail(node* n){tail = n;};

	node_container(string type_name) : nt(type_name){};

	void delete_node(node* n)
	{
		int to_delete_index = -1;
		for(int i=0; i<nodes.size(); i++)
			if(nodes[i]==n)
				to_delete_index = i;
		if(to_delete_index>=0)
		{
			node* s = nodes[to_delete_index]->get_successor();
			node* p = nodes[to_delete_index]->get_predecessor();
			
			if(s!=NULL)
				s->set_predecessor(p);	
			if(p!=NULL)
				p->set_successor(s);

			if(s==NULL)
				tail = p;
			if(p==NULL)
				head = s;

			delete nodes[to_delete_index];
			nodes.erase(nodes.begin()+to_delete_index);
		}
	};
	~node_container()
	{
		for(int i=0; i<nodes.size(); i++)
			delete nodes[i];
	};
};

class database
{
	vector<node_container*>	node_containers;
public:
	database(){};
	vector<node_container*>& get_node_containers(){return node_containers;};
	void clear()
	{
		for(int i=0; i<node_containers.size(); i++)
			delete node_containers[i];
		node_containers.clear();
	};
	bool there_is_no_data()
	{
		return (node_containers.size()==0 || node_containers[0]->get_nodes().size() == 0);
	};
	~database()
	{
		clear();
	};
};

#endif
