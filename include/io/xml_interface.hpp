#ifndef XML_INTERFACE_HPP
#define XML_INTERFACE_HPP

#include <iostream>
#include <fstream>
#include <sstream>

#include "pugixml.hpp"

#include "data/data_structures.hpp"
#include "data/database_manager.hpp"
#include "data_interaction/node_iterators.hpp"

class xml_interface
{
	database_manager& dm;
	ofstream o;
	pugi::xml_document doc;
	pugi::xml_node gml;
	pugi::xml_node categories_graph;
	pugi::xml_node nodes_graph;

public:
	xml_interface(database_manager& dm) : dm(dm){};

	bool save(string filename)
	{
		o.open(filename);
		if(o.good())
		{
			start_graphml_document();
			add_categories();
			add_nodes();
			add_links();
			flush();
			return true;
		}
		else
			return false;
	};

	bool open(string filename)
	{
		pugi::xml_parse_result r = doc.load_file(filename.c_str());
		
		if(r)
		{
			dm.db.clear();
			gml = doc.child("graphml");
			categories_graph = gml.child("graph");
			nodes_graph = categories_graph.next_sibling("graph");
			extract_categories();
			extract_nodes();
			extract_links();
			return true;
		}
		else
			return false;
	};

	void start_graphml_document()
	{
		pugi::xml_node contents_key = doc.append_child("key");
		contents_key.append_attribute("id") = "d0";
		contents_key.append_attribute("for") = "node";
		contents_key.append_attribute("attr.name") = "contents";
		contents_key.append_attribute("attr.type") = "string";	

		pugi::xml_node category_key = doc.append_child("key");
		category_key.append_attribute("id") = "d1";
		category_key.append_attribute("for") = "node";
		category_key.append_attribute("attr.name") = "category";
		category_key.append_attribute("attr.type") = "string";	

		pugi::xml_node color_key = doc.append_child("key");
		color_key.append_attribute("id") = "d2";
		color_key.append_attribute("for") = "node";
		color_key.append_attribute("attr.name") = "color";
		color_key.append_attribute("attr.type") = "int";	

		pugi::xml_node relation_key = doc.append_child("key");
		relation_key.append_attribute("id") = "d3";
		relation_key.append_attribute("for") = "edge";
		relation_key.append_attribute("attr.name") = "relation";
		relation_key.append_attribute("attr.type") = "string";	

		gml = doc.append_child("graphml");

		categories_graph = gml.append_child("graph");
		categories_graph.append_attribute("id") = "C";
		categories_graph.append_attribute("edgedefault") = "directed";

		nodes_graph = gml.append_child("graph");
		nodes_graph.append_attribute("id") = "N";
		nodes_graph.append_attribute("edgedefault") = "directed";
	};

	void add_categories()
	{
		
		vector<node_container*>& nc = dm.db.get_node_containers();	
		for(int i=0; i<nc.size(); i++)
		{
			nc[i]->get_node_type().set_id("c" + to_string(i));
			add_category(nc[i]);
		}
	};

	void add_category(node_container* nc)
	{
		node_type& nt = nc->get_node_type();
		pugi::xml_node xc = categories_graph.append_child("node");
		xc.append_attribute("id") = nt.get_id().c_str();

		pugi::xml_node d = xc.append_child("data");
		d.append_attribute("key") = "d1";
		d.append_child(pugi::node_pcdata).set_value(nt.get_name().c_str());

		pugi::xml_node cl = xc.append_child("data");
		cl.append_attribute("key") = "d2";
		cl.append_child(pugi::node_pcdata).set_value( to_string(nt.get_color_index()).c_str() );

	};

	void add_nodes()
	{
		vector<node_container*>& nc = dm.db.get_node_containers();
		for(int i=0; i<nc.size(); i++)
		{
			single_category_iterator it(nc[i]->get_head());

			node* next;
			int j=0;
			while((next=it.next_node())!=NULL)
			{
				next->set_id("n" + to_string(j*nc.size()+i) );
				add_node(next);
				j++;
			}
			// vector<node*>& nodes = nc[i]->get_nodes();
			// for(int j=0; j<nodes.size(); j++)
			// {
			// 	nodes[j]->set_id("n" + to_string(j*nc.size() + i));
			// 	add_node(nodes[j]);
			// }

		}
	};

	void add_node(node* n)
	{
		pugi::xml_node xn = nodes_graph.append_child("node");
		xn.append_attribute("id") = n->get_id().c_str();

		pugi::xml_node d = xn.append_child("data");
		d.append_attribute("key") = "d0";
		d.append_child(pugi::node_pcdata).set_value(n->contents().c_str());

		pugi::xml_node c = xn.append_child("data");
		c.append_attribute("key") = "d1";
		c.append_child(pugi::node_pcdata).set_value(n->get_node_type().get_name().c_str());
	};

	void add_links()
	{
		vector<node_container*>& nc = dm.db.get_node_containers();
		for(int i=0; i<nc.size(); i++)
		{
			vector<node*>& nodes = nc[i]->get_nodes();
			for(int j=0; j< nodes.size(); j++)
			{
				add_links(nodes[j]);
			}
		}
	};

	void add_links(node* n)
	{
		vector<directed_link*>& links = n->get_links();
		for(int i=links.size()-1; i>=0;i--)
		{
			add_link(n, links[i]);			
		}
	};

	void add_link(node* n, directed_link* l)
	{
		pugi::xml_node xn = nodes_graph.append_child("edge");
		xn.append_attribute("id") = (n->get_id() + l->get_end_node()->get_id()).c_str();
		xn.append_attribute("source") = n->get_id().c_str();
		xn.append_attribute("target") = l->get_end_node()->get_id().c_str();

		pugi::xml_node c = xn.append_child("data");
		c.append_attribute("key") = "d3";
		c.append_child(pugi::node_pcdata).set_value(l->get_name().c_str());

	}

	void flush()
	{
		doc.save(o);
		o.close();
	};

	void extract_categories()
	{	
		for (pugi::xml_node n = categories_graph.first_child(); n; n = n.next_sibling())
		{
			pugi::xml_node name = n.first_child();
			pugi::xml_node color = name.next_sibling();
			node_container* c = dm.add_category(name.child_value());

			stringstream ss;
			ss << color.child_value();
			int val;
			ss >> val;

			c->get_node_type().set_color(val);
			
		}
	};

	void extract_nodes()
	{
		for(pugi::xml_node n = nodes_graph.child("node"); n; n=n.next_sibling("node"))
		{
			pugi::xml_node contents = n.first_child();
			pugi::xml_node cat = contents.next_sibling();
			
			node* my_node = dm.add_node(contents.child_value(), cat.child_value());
			if(my_node!=NULL)
				my_node->set_id(n.attribute("id").value());
		}
	};

	void extract_links()
	{
		for(pugi::xml_node e = nodes_graph.child("edge"); e; e=e.next_sibling("edge"))
		{
			string s = e.attribute("source").value();
			string t = e.attribute("target").value();
			string relation_description = e.first_child().child_value();
			if(!dm.are_linked_by_id(s,t))
				dm.register_link_by_ids(s,t,relation_description);
		}
	};
};

#endif