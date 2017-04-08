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
	{ // title person date item_type sec subsec subsubsec key1 key2 key3 item_num notation text
		node_container* title=		dm.add_category("title");
		node_container* person=		dm.add_category("person");
		node_container* date=		dm.add_category("date");
		node_container* item_type=	dm.add_category("item type");
		node_container* sec=		dm.add_category("sec");
		node_container* subsec=		dm.add_category("subsec");
		node_container* subsubsec=	dm.add_category("subsubsec");
		node_container* key1=		dm.add_category("key1");
		node_container* key2=		dm.add_category("key2");
		node_container* key3=		dm.add_category("key3");
		node_container* item_num=	dm.add_category("item num");
		node_container* notation=	dm.add_category("notation");
		node_container* text=		dm.add_category("text");
		
		title->get_node_type().set_color(1);
		person->get_node_type().set_color(2);
		date->get_node_type().set_color(3);
		item_type->get_node_type().set_color(4);
		sec->get_node_type().set_color(5);
		subsec->get_node_type().set_color(6);
		subsubsec->get_node_type().set_color(7);
		key1->get_node_type().set_color(0);
		key2->get_node_type().set_color(1);
		key3->get_node_type().set_color(2);
		item_num->get_node_type().set_color(3);
		notation->get_node_type().set_color(4);
		text->get_node_type().set_color(5);

		dm.add_node("definition","item type");
		dm.add_node("proposition","item type");
		dm.add_node("theorem","item type");
		dm.add_node("corollary","item type");
		dm.add_node("lemma","item type");
		dm.add_node("conjecture","item type");
		dm.add_node("claim","item type");
		dm.add_node("axiom","item type");
		dm.add_node("example","item type");
		dm.add_node("remark","item type");
	};

	void get_simple_metadata()
	{
		//Try to grab title, author, date, proper names, list of environments.
		
		node* t = dm.add_node("...","title");

		// boost::regex re("\\title\s*\{(.*)\}");
		// ...boost::regex_search(s,re);

		// regex on large string, pass by reference?

		node* a = dm.add_node("...","person");

		// loop...
		// node* it = dm.add_node(,"item type");

		node* d = dm.add_node("...","date");

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
