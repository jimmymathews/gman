#ifndef TEX_PARSER_HPP
#define TEX_PARSER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <boost/regex.hpp>
#include "data/data_structures.hpp"
#include "data/database_manager.hpp"
#include "data_interaction/node_iterators.hpp"

class keyphrase
{
	string contents;
public:
	float weight=1.0;
	keyphrase(string& contents, float weight) : contents(contents), weight(weight){};
};

struct keyness_comparator
{
	bool operator()(const keyphrase& lhs, const keyphrase& rhs) const
	{
		return lhs.weight < rhs.weight;
	};
};

class tex_parser
{
	database_manager& dm;
	ifstream f;
	bool merge_mode = false;

	int num_phases = 100;
	int min_freq[10] = {-1,15,7,6,3,3,2,2,2,2};
	int max_words_per_phrase = sizeof(min_freq)/sizeof(*min_freq);
    float distribution_weights[3] = {0.2,0.3,0.5};
	std::priority_queue<keyphrase, std::vector<keyphrase>, keyness_comparator> q;

public:
	tex_parser(database_manager& dm) : dm(dm){};

	void set_merge_mode()
	{
		merge_mode=true;
	};

	bool open(string filename)
	{
		f.open(filename);
		if(f.is_open())
		{
			if(!merge_mode)
				dm.db.clear();
			extract_graph_data_from_tex();
			return true;
		}
		return false;
	};

	void extract_graph_data_from_tex()
	{
		define_categories();
		get_simple_metadata();
		strip_to_document();
		render_or_elide_math_environments_except_notations();
		strip_unhandled_environments();
		get_and_mark_keyphrases();
		sort_keyphrases();
		unmark_and_push_links();
	};

	void define_categories()
	{	
		node_container* c=dm.add_category("dummy cat"); //*
		c->get_node_type().set_color(1); //*
		dm.add_node("mm","dummy cat"); //*		
	};

	void get_simple_metadata()
	{
		// node* n1 = dm.add_node("m1","dummy cat");
		// n1->set_id("A");
		// node* n2 = dm.add_node("m2","dummy cat");
		// n2->set_id("B");
		// node* n3 = dm.add_node("d1","dummy dog");
		// n3->set_id("C");
		// // if(n!=NULL)
		// // 	n->set_id("an identifying string that I guess is used instead of the whole string sometimes for efficiency");

	};

	void strip_to_document()
	{

	};

	void render_or_elide_math_environments_except_notations()
	{

	};

	void strip_unhandled_environments()
	{

	};

	void get_and_mark_keyphrases()
	{
		reset_auxiliary_variables();
		// q.push(elt);
		// q.push(elt);
	};

	void sort_keyphrases()
	{
		// while(!q.empty())
		// {
		// 	process(q.top());
		// 	q.pop();
		// }

		// node* n1 = dm.add_node("m1","dummy cat");
		// n1->set_id("A");
		// node* n2 = dm.add_node("m2","dummy cat");
		// n2->set_id("B");
		// node* n3 = dm.add_node("d1","dummy dog");
		// n3->set_id("C");
		// // if(n!=NULL)
		// // 	n->set_id("an identifying string that I guess is used instead of the whole string sometimes for efficiency");

	};

	void unmark_and_push_links()
	{
		// // if(!dm.are_linked_by_id("A","C"))
		// dm.register_link_by_ids("A","C","relation description");  //only goes one way!
		// dm.register_link_by_ids("C","A","relation description");  //only goes one way!

		//unmark keyphrases one by one
		//unmark notations one by one
	};

	void reset_auxiliary_variables()
	{
		q = std::priority_queue<keyphrase, std::vector<keyphrase>, keyness_comparator>();
	};

	float keyness(	int frequency,		int num_words,			int emph_occurences,
					int def_occurences,	int thm_occurences,		int prop_occurences,
					int cor_occurences, int conj_occurences,	int lem_occurences,
					int clm_occurences, int rmk_occurences,		int other_occurences,
					int sec_occurences, int ssec_occurrences,	int sssec_occurences)
	{
		//The first number is an "importance" compared to unit 1.0, the denominator is the expected, normal value.
		float a[15]={	1.0/5.0,	0.2/3.0,	3.0/0.4,
						1.3/0.8,	1.5/0.8,	1.3/0.8,
						1.1/0.8,	1.3/0.5,	1.3/0.8,
						1.3/0.8,	1.3/1.0,	1.0/0.9,
						2.2/0.4,	1.8/0.4,	1.6/0.4};
		return	a[0]*frequency+			a[1]*num_words+			a[2]*emph_occurences+
				a[3]*def_occurences+	a[4]*thm_occurences+	a[5]*prop_occurences+
				a[6]*cor_occurences+	a[7]*conj_occurences+	a[8]*lem_occurences+
				a[9]*clm_occurences+	a[10]*rmk_occurences+	a[11]*other_occurences+
				a[12]*sec_occurences+	a[13]*ssec_occurrences+	a[14]*sssec_occurences;
	};
};


#endif
