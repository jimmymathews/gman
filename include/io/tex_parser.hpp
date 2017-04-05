#ifndef TEX_PARSER_HPP
#define TEX_PARSER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include "data/data_structures.hpp"
#include "data/database_manager.hpp"
#include "data_interaction/node_iterators.hpp"

class tex_parser
{
	database_manager& dm;

public:
	tex_parser(database_manager& dm) : dm(dm){};

	bool open(string filename)
	{
		ifstream f;
		f.open(filename);
		if(f.is_open())
		{
			dm.db.clear();
			define_categories();
			extract_nodes();
			extract_links();
			return true;
		}
		return false;
	};

	void define_categories()
	{	
		node_container* c=dm.add_category("dummy cat");
		c->get_node_type().set_color(1);
		node_container* d=dm.add_category("dummy dog");
		d->get_node_type().set_color(2);
	};

	void extract_nodes()
	{
		node* n1 = dm.add_node("m1","dummy cat");
		n1->set_id("A");
		node* n2 = dm.add_node("m2","dummy cat");
		n2->set_id("B");
		node* n3 = dm.add_node("d1","dummy dog");
		n3->set_id("C");
		// if(n!=NULL)
		// 	n->set_id("an identifying string that I guess is used instead of the whole string sometimes for efficiency");
	};

	void extract_links()
	{
		// if(!dm.are_linked_by_id("A","C"))
		dm.register_link_by_ids("A","C","relation description");  //only goes one way!
		dm.register_link_by_ids("C","A","relation description");  //only goes one way!
	};
};


#endif
