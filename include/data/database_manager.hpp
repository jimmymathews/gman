#ifndef DATABASE_MANAGER_HPP
#define DATABASE_MANAGER_HPP

#include "data/data_structures.hpp"
#include <string>
#include <vector>
using namespace std;

class database_manager
{
	void populate_default_database();
	node*			get_node(string contents, string category);
	node*			get_node(string contents, node_container* category);
	void add_node_to_linked_list(node_container* nc, node* new_node, bool prepend);

public:
	database db;
	database_manager(){populate_default_database();};

	node_container*	get_category(string category);
	node_container* add_category(string name);
	void delete_category(string name);
	node* add_node_and_insert_before(string contents, string category, node* n);
	node* add_node(string contents, string category);
	node* add_simple_node(string contents, string category);
	node* add_node(string contents, string category, bool prepend);
	void register_link_by_ids(string id1, string id2, string relation_description, string r2, string r3, string r4);
	bool are_linked_by_id(string s, string t);

	void swap_nodes(node* n1, node* n2);
};

#endif

