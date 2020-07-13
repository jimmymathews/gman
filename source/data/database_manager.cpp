#include "data/database_manager.hpp"

//Public

node_container*	database_manager::get_category(string category)
{
	vector<node_container*> node_containers = db.get_node_containers();
	for(int i=0; i<node_containers.size(); i++)
	{
		if(category == node_containers[i]->get_node_type().get_name())
			return node_containers[i];
	}
	return NULL;
}

node_container* database_manager::add_category(string name)
{
	if(get_category(name)!=NULL)
		return NULL;//should this return get_category(name)?
	node_container* new_category = new node_container(name);
	node_type& nt = new_category->get_node_type();
	db.get_node_containers().push_back(new_category);
	return new_category;
}

void database_manager::delete_category(string name)
{
	node_container* nc = get_category(name);
	if(nc!= NULL)
	{
		int to_delete_index=-1;
		vector<node_container*>& vec = db.get_node_containers();
		for(int i=0; i<vec.size();i++)
			if(vec[i]==nc)
				to_delete_index = i;
		if(to_delete_index>=0)
		{
			delete vec[to_delete_index];
			vec.erase(vec.begin()+to_delete_index);
		}
	}
}

node* database_manager::add_node_and_insert_before(string contents, string category, node* n)
{
	node_container* nc = get_category(category);
	if(nc!=NULL && get_node(contents, category)==NULL)
	{
		node* new_node = new node(contents, nc->get_node_type());
		
		node* p = n->get_predecessor();
		new_node->set_predecessor(p);
		if(p!=NULL)
			p->set_successor(new_node);
		new_node->set_successor(n);
		if(n!=NULL)
			n->set_predecessor(new_node);

		if(n == nc->get_head())
			nc->set_head(new_node);

		return new_node;
	}
	return NULL;	
}

node* database_manager::add_node(string contents, string category)
{
	return add_node(contents,category,false);
}

node* database_manager::add_simple_node(string contents, string category)
{
	node* sn = add_node(contents,category,false);
	if(sn!=NULL)
		sn->set_id(contents);
	return sn;
}

node* database_manager::add_node(string	contents, string category, bool prepend)
{
	node_container* nc = get_category(category);
	if(nc!=NULL && get_node(contents, category)==NULL)
	{
		node* new_node = new node(contents, nc->get_node_type());
		add_node_to_linked_list(nc, new_node, prepend);
		return new_node;
	}
	return NULL; // return get_node(...) if not null?
}

void database_manager::register_link_by_ids(string id1, string id2, string relation_description, string r2, string r3, string r4)
{
	node* n1;
	node* n2;
	vector<node_container*>& nc = db.get_node_containers();
	for(int i=0; i<nc.size(); i++)
	{
		vector<node*>& ni = nc[i]->get_nodes();
		for(int j=0; j<ni.size(); j++)
		{
			if(ni[j]->get_id() == id1)
				n1 = ni[j];
			if(ni[j]->get_id() == id2)
				n2 = ni[j];
		}
	}
	n1->one_way_link(relation_description, r2, r3, r4, n2);
	n2->one_way_link(relation_description, r2, r3, r4, n1);
}

bool database_manager::are_linked_by_id(string id1, string id2)
{
	node* n1;
	node* n2;
	vector<node_container*>& nc = db.get_node_containers();
	for(int i=0; i<nc.size(); i++)
	{
		vector<node*>& ni = nc[i]->get_nodes();
		for(int j=0; j<ni.size(); j++)
		{
			if(ni[j]->get_id() == id1)
				n1 = ni[j];
			if(ni[j]->get_id() == id2)
				n2 = ni[j];
		}
	}
	return n1->linked_to(n2);
}





//Internal

void database_manager::populate_default_database()
{
	node_container* points = add_category("Fano point");
	node_container* lines = add_category("Fano line");
	
	points->get_node_type().set_color(1);
	lines->get_node_type().set_color(2);

	node* a = add_node("001","Fano point");
	node* b = add_node("010","Fano point");
	node* c = add_node("100","Fano point");
	node* A = add_node("110","Fano point");
	node* B = add_node("101","Fano point");
	node* C = add_node("011","Fano point");
	node* x = add_node("111","Fano point");

	node* ab= add_node("001 011 010","Fano line");
	node* bc= add_node("010 110 100","Fano line");
	node* ca= add_node("100 101 001","Fano line");
	node* aA= add_node("001 111 110","Fano line");
	node* bB= add_node("010 111 101","Fano line");
	node* cC= add_node("100 111 011","Fano line");
	node* X = add_node("110 101 011","Fano line");

	a->link(ab);
	a->link(ca);
	a->link(aA);

	b->link(bc);
	b->link(ab);
	b->link(bB);

	c->link(ca);
	c->link(bc);
	c->link(cC);

	A->link(aA);
	A->link(bc);
	A->link(X);

	B->link(bB);
	B->link(ca);
	B->link(X);

	C->link(cC);
	C->link(ab);
	C->link(X);

	x->link(aA);
	x->link(bB);
	x->link(cC);
}

node* database_manager::get_node(string	contents, string	category)
{
	node_container* c = get_category(category);
	if(c==NULL)
		return NULL;
	return get_node(contents, c);
}

node* database_manager::get_node(string contents, node_container* category)
{
	for(int i=0; i<category->get_nodes().size(); i++)
	{
		if(category->get_nodes()[i]->contents() == contents)
			return category->get_nodes()[i];
	}
	return NULL;
}

void database_manager::add_node_to_linked_list(node_container* nc, node* new_node, bool prepend)
{
	vector<node*>& nodes = nc->get_nodes();
	
	if(nodes.size()==0)
	{
		nodes.push_back(new_node);
		nc->set_head(new_node);
		nc->set_tail(new_node);
	}
	else
	{
		nodes.push_back(new_node);
		if(!prepend)
		{
			node* previous_tail = nc->get_tail();
			node* previous_pen_tail = previous_tail->get_predecessor();
			nc->set_tail(new_node);
			new_node->set_successor(NULL);
			new_node->set_predecessor(previous_tail);
			previous_tail->set_successor(new_node);
		}
		else
		{
			node* previous_head = nc->get_head();
			node* previous_pen_head = previous_head->get_successor();
			nc->set_head(new_node);
			new_node->set_predecessor(NULL);
			new_node->set_successor(previous_head);
			previous_head->set_predecessor(new_node);
		}
	}
}

void database_manager::swap_nodes(node* n1, node* n2)
{
	if( !(n1->get_node_type() == n2->get_node_type()) )
		return;

	node_container* nc = get_category(n1->get_node_type().get_name());
	node* head = nc->get_head();
	node* tail = nc->get_tail();
	if(n1 == head)
		nc->set_head(n2);
	if(n2 == head)
		nc->set_head(n1);
	if(n1 == tail)
		nc->set_tail(n2);
	if(n2 == tail)
		nc->set_tail(n1);

	node* p1 = n1->get_predecessor();	
	node* p2 = n2->get_predecessor();
	node* s1 = n1->get_successor();
	node* s2 = n2->get_successor();

	if(s1 == n2 && n1 == p2)
	{
		n1->set_predecessor(n2);
		n2->set_successor(n1);

		n2->set_predecessor(p1);
		if(p1!=NULL)
			p1->set_successor(n2);

		n1->set_successor(s2);
		if(s2!=NULL)
			s2->set_predecessor(n1);
	}
	else
	if(s2 == n1 && n2 == p1)
	{
		n2->set_predecessor(n1);
		n1->set_successor(n2);

		n1->set_predecessor(p2);
		if(p2!=NULL)
			p2->set_successor(n1);

		n2->set_successor(s1);
		if(s1!=NULL)
			s1->set_predecessor(n2);
	}
	else
	{
		n1->set_predecessor(p2);
		if(p2!=NULL)
			p2->set_successor(n1);

		n2->set_predecessor(p1);
		if(p1!=NULL)
			p1->set_successor(n2);

		n1->set_successor(s2);
		if(s2!=NULL)
			s2->set_predecessor(n1);

		n2->set_successor(s1);
		if(s1!=NULL)
			s1->set_predecessor(n2);
	}

}

void database_manager::sort(){
    vector<node_container*> ncs = db.get_node_containers();
    for(int i=0; i<ncs.size(); i++) {
        ncs[i]->sort();
    }
}



















