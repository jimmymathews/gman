#ifndef TEX_PARSER_HPP
#define TEX_PARSER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <boost/regex.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
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
	string document;

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
		setup();
		define_categories();
		strip_to_document();
		render_or_elide_math_environments_except_notations();
		get_simple_metadata();
		strip_unhandled_environments();
		get_and_mark_keyphrases();
		sort_keyphrases();
		unmark_and_push_links();
	};

	void setup()
	{
		std::stringstream ss;
		ss << f.rdbuf();
		document = ss.str();
	};

	void define_categories()
	{ // title name date item_type sec subsec subsubsec key1 key2 key3 item_num notation text
		node_container* title=		dm.add_category("title");
		node_container* name=		dm.add_category("name");
		node_container* inlinemath=	dm.add_category("inline");
		node_container* equation=	dm.add_category("equation");
		node_container* date=		dm.add_category("date");
		node_container* item_type=	dm.add_category("item type");
		node_container* sec=		dm.add_category("sec");
		node_container* subsec=		dm.add_category("subsec");
		node_container* subsubsec=	dm.add_category("subsubsec");
		node_container* key1=		dm.add_category("key1");
		node_container* key2=		dm.add_category("key2");
		node_container* key3=		dm.add_category("key3");
		node_container* item_num=	dm.add_category("item num");
		node_container* text=		dm.add_category("text");
		
		title->get_node_type().set_color(1);
		name->get_node_type().set_color(2);
		inlinemath->get_node_type().set_color(3);
		equation->get_node_type().set_color(4);
		date->get_node_type().set_color(5);
		item_type->get_node_type().set_color(4);
		sec->get_node_type().set_color(5);
		subsec->get_node_type().set_color(6);
		subsubsec->get_node_type().set_color(7);
		key1->get_node_type().set_color(0);
		key2->get_node_type().set_color(1);
		key3->get_node_type().set_color(2);
		item_num->get_node_type().set_color(3);
		text->get_node_type().set_color(5);

		dm.add_simple_node("definition","item type");
		dm.add_simple_node("proposition","item type");
		dm.add_simple_node("theorem","item type");
		dm.add_simple_node("corollary","item type");
		dm.add_simple_node("lemma","item type");
		dm.add_simple_node("conjecture","item type");
		dm.add_simple_node("claim","item type");
		dm.add_simple_node("axiom","item type");
		dm.add_simple_node("example","item type");
		dm.add_simple_node("remark","item type");
	};

	void get_simple_metadata()
	{
		//Try to grab title, author, date, proper names, list of environments.

		boost::regex re = boost::regex(R"(\\title\s*\{([^\}]*)\})");
		boost::cmatch matches;
		boost::regex_search(document.c_str(),matches,re);
		node* t = dm.add_simple_node(matches[1].str(),"title");

		re = boost::regex(R"(\\author\{([^\}]*)\})");
		boost::regex_search(document.c_str(),matches,re);
		string a = matches[1];

		re = boost::regex(R"([A-Z](\w+|\.)\s[A-Z](\w+|\.)(\s[A-Z](\w+|\.))?)");    //This is an OK name finding pattern
		boost::sregex_iterator b = boost::sregex_iterator(a.begin(), a.end(), re);
    	boost::sregex_iterator e = boost::sregex_iterator();
	    for (boost::sregex_iterator i = b; i != e; ++i)
        	dm.add_simple_node((*i)[0].str(),"name");
	
	};

	void strip_to_document()
	{
		boost::regex begdoc = boost::regex(R"(\\begin\{document\})");
		boost::regex enddoc = boost::regex(R"(\\end\{document\})");
		boost::cmatch b;
		boost::cmatch e;
		boost::regex_search(document.c_str(),b,begdoc);
		boost::regex_search(document.c_str(),e,enddoc);
		document=document.substr(b.position()+16,e.position()-b.position()-16);
		// dm.add_node(document,"text");//test
	};

	void render_or_elide_math_environments_except_notations()
	{
		boost::regex re= boost::regex(R"(%.*?\n)");
		document = regex_replace(document, re, "");
		re= boost::regex(R"((\w|\.|,|\$|\(|\))[\t ]{0,50}\n[\t ]{0,50}([^\n]))");
		document = regex_replace(document, re, "$1 $2");

		boost::regex inlineformula = boost::regex(R"(\$([^\$])+\$)"); //size limit?
		boost::function<std::string (boost::match_results<std::string::const_iterator>)> fun1 = boost::bind(&tex_parser::render_inline_environment, this, _1);
		document = boost::regex_replace(document, inlineformula, fun1);

		boost::regex eqformula = boost::regex(R"(\\begin\{(equation|eqnarry)\*?\}(.*?)\\end\{(equation|eqnarry)\*?\})"); //size limit?
		boost::function<std::string (boost::match_results<std::string::const_iterator>)> fun2 = boost::bind(&tex_parser::render_equation_environment, this, _1);
		document = boost::regex_replace(document, eqformula, fun2);

		// boost::sregex_iterator b = boost::sregex_iterator(document.begin(), document.end(), inlineformula);
	//	boost::sregex_iterator e = boost::sregex_iterator();
	//	for (boost::sregex_iterator i = b; i != e; ++i)
	//	{
	//		string con = (*i)[0].str();
  	//		dm.add_simple_node(con.substr(1,con.size()-2),"notation");
	//	}
	};

	string code_for_greek(int ch)
	{
		//	32~126: 						regular chars, includes, numerals 48~57, symbols before that, and 65~122 letters
		//	65~122 + 128 = 193~250:			greek letters
		unsigned char char_cache = ch;
		char_cache = ch + 128;
		return string(1,char_cache);
	};

	string code_for_superscript(int ch)
	{
		if(ch==' ')
		return "";

		//	48~57 - (48-14) = 14~23:		superscript numerals
		//	24,25,27,28:					special +/- sup and superscripts	
		unsigned char char_cache = ch;
		if(ch >= 48 && ch <=57)
			char_cache = ch - (48-14);
		if(ch == 43)
			char_cache = 24;	//+
		if(ch == 45)
			char_cache = 25;	//-

		return string(1,char_cache);		
	};

	string code_for_subscript(int ch)
	{
		if(ch==' ')
		return "";

		//	48~57 + (128-48) = 128~137:		subscript numerals
		//	97~122 + (138-97) = 138~163:	subscript lowercase letters
		//	24,25,27,28:					special +/- sup and superscripts	
		unsigned char char_cache = ch;
		if( (ch>=48 && ch<=57))
			char_cache = ch + (128-48);
		if(ch=='a' || ch=='e' || ch=='h' || ch=='i' || ch=='j' ||ch =='k' || ch=='l'|| ch=='m'|| ch=='n'|| ch=='p'|| ch=='s'|| ch=='t'|| ch=='u'|| ch=='v' || ch =='x')
			char_cache = ch + (138-97);
		if(ch == 43)
			char_cache = 27;	//+
		if(ch == 45)
			char_cache = 28;	//-
		return string(1,char_cache);
	};

	string render_equation_environment(const boost::smatch& match)
	{
		string in = render_math_environment(match[2].str()); 
		if(in.size()<5000) //?
			dm.add_simple_node(in,"equation");
		return "\n" + in +"\n";
	};

	string render_inline_environment(const boost::smatch& match)
	{
		string in = render_math_environment(match[0].str());
		if(in.size()<45)
			dm.add_simple_node(in,"inline");
		return in;
	};

	string render_math_environment(string in)
	{
		boost::regex re;

		// re = boost::regex(R"(\\[t]?frac(\{((?>[^{}]+|(?1))*)\})\{)");                      //frac requires care in case it is nested!
		// in = regex_replace(in, re, "myspecialfrac$1termmyspecialfracnow{");
		// re = boost::regex(R"(myspecialfrac\{(.*?)\}termmyspecialfracnow(\{((?>[^{}]+|(?1))*)\}))");
		// in = regex_replace(in, re, " $1/($3) ");

		re = boost::regex(R"(\\geq)");
		in = regex_replace(in, re, ">=");
		re = boost::regex(R"(\\leq)");
		in = regex_replace(in, re, "<=");
		re = boost::regex(R"(\\neq)");
		in = regex_replace(in, re, "!=");
		re = boost::regex(R"(\\wedge)");
		in = regex_replace(in, re, code_for_greek('L'));
		re = boost::regex(R"(\\rightarrow|\\mapsto)");
		in = regex_replace(in, re, "->");
		re = boost::regex(R"(\\Rightarrow)");
		in = regex_replace(in, re, "=>");
		re = boost::regex(R"(\\leftarrow)");
		in = regex_replace(in, re, "<-");
		re = boost::regex(R"(\\Leftarrow)");
		in = regex_replace(in, re, "<=");
		re = boost::regex(R"(\\longmapsto)");
		in = regex_replace(in, re, "-->");
		re = boost::regex(R"(\\leftrightarrow)");
		in = regex_replace(in, re, "<->");
		re = boost::regex(R"(\\Leftrightarrow)");
		in = regex_replace(in, re, "<=>");
		re = boost::regex(R"(\\equiv)");
		in = regex_replace(in, re, "<=>");
		re = boost::regex(R"(\\partial)");
		string cd = string(1,168);
		in = regex_replace(in, re, cd);
		re = boost::regex(R"(\\otimes)");
		cd = string(1,169);
		in = regex_replace(in, re, cd);
		re = boost::regex(R"(\\ast|\\star)");
		in = regex_replace(in, re, "*");
		re = boost::regex(R"(\\oplus)");
		in = regex_replace(in, re, "+");
		re = boost::regex(R"(\\cong|\\equiv)");
		in = regex_replace(in, re, "=");
		re = boost::regex(R"(\\simeq|\\sim)");
		in = regex_replace(in, re, "~");
		re = boost::regex(R"(\\cdot)");
		in = regex_replace(in, re, ".");
		re = boost::regex(R"(\\prime)");
		in = regex_replace(in, re, "'");

		re = boost::regex(R"(\\right)");
		in = regex_replace(in, re, "");
		re = boost::regex(R"(\\left)");
		in = regex_replace(in, re, "");
		re = boost::regex(R"(\\langle)");
		in = regex_replace(in, re, "<");
		re = boost::regex(R"(\\rangle)");
		in = regex_replace(in, re, ">");

		re = boost::regex(R"(\\alpha)");
		in = regex_replace(in, re, code_for_greek('a'));
		re = boost::regex(R"(\\beta)");
		in = regex_replace(in, re, code_for_greek('b'));
		re = boost::regex(R"(\\psi)");
		in = regex_replace(in, re, code_for_greek('c'));
		re = boost::regex(R"(\\delta)");
		in = regex_replace(in, re, code_for_greek('d'));
		re = boost::regex(R"(\\epsilon|\\varepsilon)");
		in = regex_replace(in, re, code_for_greek('e'));
		re = boost::regex(R"(\\phi|\\varphi)");
		in = regex_replace(in, re, code_for_greek('f'));
		re = boost::regex(R"(\\gamma)");
		in = regex_replace(in, re, code_for_greek('g'));
		re = boost::regex(R"(\\theta)");
		in = regex_replace(in, re, code_for_greek('h'));
		re = boost::regex(R"(\\iota)");
		in = regex_replace(in, re, code_for_greek('i'));
		// re = boost::regex(R"(j)");
		// in = regex_replace(in, re, code_for_greek('j'));
		re = boost::regex(R"(\\kappa)");
		in = regex_replace(in, re, code_for_greek('k'));
		re = boost::regex(R"(\\lambda)");
		in = regex_replace(in, re, code_for_greek('l'));
		re = boost::regex(R"(\\mu)");
		in = regex_replace(in, re, code_for_greek('m'));
		re = boost::regex(R"(\\nu)");
		in = regex_replace(in, re, code_for_greek('n'));
		re = boost::regex(R"(\\omega)");
		in = regex_replace(in, re, code_for_greek('o'));
		re = boost::regex(R"(\\pi)");
		in = regex_replace(in, re, code_for_greek('p'));
		re = boost::regex(R"(\\eta)");
		in = regex_replace(in, re, code_for_greek('q'));
		re = boost::regex(R"(\\rho)");
		in = regex_replace(in, re, code_for_greek('r'));
		re = boost::regex(R"(\\sigma)");
		in = regex_replace(in, re, code_for_greek('s'));
		re = boost::regex(R"(\\tau)");
		in = regex_replace(in, re, code_for_greek('t'));
		// re = boost::regex(R"(u)");
		// in = regex_replace(in, re, code_for_greek('u'));
		// re = boost::regex(R"(v)");
		// in = regex_replace(in, re, code_for_greek('v'));
		// re = boost::regex(R"(w)");
		in = regex_replace(in, re, code_for_greek('w'));
		re = boost::regex(R"(\\xi)");
		in = regex_replace(in, re, code_for_greek('x'));
		// re = boost::regex(R"(y)");
		// in = regex_replace(in, re, code_for_greek('y'));
		re = boost::regex(R"(\\zeta)");
		in = regex_replace(in, re, code_for_greek('z'));
		re = boost::regex(R"(\\Alpha)");
		in = regex_replace(in, re, code_for_greek('A'));
		re = boost::regex(R"(\\Beta)");
		in = regex_replace(in, re, code_for_greek('B'));
		re = boost::regex(R"(\\Psi)");
		in = regex_replace(in, re, code_for_greek('C'));
		re = boost::regex(R"(\\Delta)");
		in = regex_replace(in, re, code_for_greek('D'));
		re = boost::regex(R"(\\Epsilon)");
		in = regex_replace(in, re, code_for_greek('E'));
		re = boost::regex(R"(\\Phi)");
		in = regex_replace(in, re, code_for_greek('F'));
		re = boost::regex(R"(\\Gamma)");
		in = regex_replace(in, re, code_for_greek('G'));
		re = boost::regex(R"(\\Theta)");
		in = regex_replace(in, re, code_for_greek('H'));
		re = boost::regex(R"(\\Iota)");
		in = regex_replace(in, re, code_for_greek('I'));
		// re = boost::regex(R"(J)");
		// in = regex_replace(in, re, code_for_greek('j'));
		re = boost::regex(R"(\\Kappa)");
		in = regex_replace(in, re, code_for_greek('K'));
		re = boost::regex(R"(\\Lambda)");
		in = regex_replace(in, re, code_for_greek('L'));
		re = boost::regex(R"(\\Mu)");
		in = regex_replace(in, re, code_for_greek('M'));
		re = boost::regex(R"(\\Nu)");
		in = regex_replace(in, re, code_for_greek('N'));
		re = boost::regex(R"(\\Omega)");
		in = regex_replace(in, re, code_for_greek('O'));
		re = boost::regex(R"(\\Pi)");
		in = regex_replace(in, re, code_for_greek('P'));
		re = boost::regex(R"(\\Eta)");
		in = regex_replace(in, re, code_for_greek('Q'));
		re = boost::regex(R"(\\Rho)");
		in = regex_replace(in, re, code_for_greek('R'));
		re = boost::regex(R"(\\Sigma)");
		in = regex_replace(in, re, code_for_greek('S'));
		re = boost::regex(R"(\\Tau)");
		in = regex_replace(in, re, code_for_greek('T'));
		// re = boost::regex(R"(u)");
		// in = regex_replace(in, re, code_for_greek('u'));
		// re = boost::regex(R"(v)");
		// in = regex_replace(in, re, code_for_greek('v'));
		// re = boost::regex(R"(w)");
		in = regex_replace(in, re, code_for_greek('W'));
		re = boost::regex(R"(\\Xi)");
		in = regex_replace(in, re, code_for_greek('X'));
		// re = boost::regex(R"(y)");
		// in = regex_replace(in, re, code_for_greek('y'));
		re = boost::regex(R"(\\Zeta)");
		in = regex_replace(in, re, code_for_greek('Z'));

		re = boost::regex(R"(\$)");		//get rid of demarcation from tex syntax
		in = regex_replace(in, re, "");

		re = boost::regex(R"(\\ )");
		in = regex_replace(in, re, " ");
		re = boost::regex(R"((\s)+)");	//get rid of extra spaces in math environment
		in = regex_replace(in, re, "");
		re = boost::regex(R"(\\,|\\!)");	// explicit spaces
		in = regex_replace(in, re, "");
		re = boost::regex(R"(\\>|\\;)");
		in = regex_replace(in, re, " ");
		re = boost::regex(R"(\\subset)");
		in = regex_replace(in, re, " contained in ");
		re = boost::regex(R"(\\supset|\\ni)");
		in = regex_replace(in, re, " contains ");
		re = boost::regex(R"(\\in)");
		in = regex_replace(in, re, " in ");

		re = boost::regex(R"(\\begin\{[bp]?matrix\}|\\end\{[bp]?matrix\})");
		in = regex_replace(in,re,"\n");
		re = boost::regex(R"(&)");	//try to render matrix type things
		in = regex_replace(in,re," ");
		re = boost::regex(R"(\\\\)");
		in = regex_replace(in,re,"\n");

		re = boost::regex(R"([\.|,|!|;|:]\$)");	//get rid of trailing punctuation
		in = regex_replace(in, re, "");

		re = boost::regex(R"(\^\{([\w\s]+)\}|\^([\w\s]))");  //render superscript
		boost::function<std::string (boost::match_results<std::string::const_iterator>)> fun1 = boost::bind(&tex_parser::superscript_string, this, _1);
		in = boost::regex_replace(in, re, fun1);

		re = boost::regex(R"(_\{([\w|\s]+)\}|_([\w|\s]))");  //render subscript
		boost::function<std::string (boost::match_results<std::string::const_iterator>)> fun2 = boost::bind(&tex_parser::subscript_string, this, _1);
		in = boost::regex_replace(in, re, fun2);

		re = boost::regex(R"(_\{([^\}]*)\}|\^{([^\}]*)\})");  //erase notation around unrendered super/sub
		in = regex_replace(in, re, "$2");
		
		re = boost::regex(R"(\\label\{[\w\s]{1,150}\})");  //erase notation
		in = regex_replace(in, re, "");

		re = boost::regex(R"(\\mathb[fb]\{([^\}]*\})|\\bf\{([^\}]*)\})");
		in = regex_replace(in, re, "$1");
		re = boost::regex(R"(\\operatorname\{([^\}]*)\})");
		in = regex_replace(in, re, "$1 ");
		re = boost::regex(R"(\\tilde\{([^\}]*)\}|\\widetilde\{([^\}]*)\})");
		in = regex_replace(in, re, "$1~");

		// re = boost::regex(R"(\\[t]?frac\{([^\}]*)\}\{([^\}]*)\})");  //wrong

		re = boost::regex(R"(\\\{)");  //render brackets
		in = regex_replace(in, re, "{");
		re = boost::regex(R"(\\\})");
		in = regex_replace(in, re, "}");

		//ref label

		// re = boost::regex(R"(\\(\w)+)");
		// in = regex_replace(in, re, "");

		// re = boost::regex(R"(\\|%)");		//shouldn't do this; takes away command names regardless of arguments; better to handle the commands?
		// in = regex_replace(in, re, "");
		
		//still need otimes, nabla,  mathbb,  ast , subset, in , frac
		// ... need to learn user-defined macros, at least the simples ones like my \ra as \rightarrow!
		// subscript/superscript functionality breaks if + or -, perhaps other symbols too?
		// definitely way to many things are created, need to break it down; inline math, versus equation...
		return in;
	};

	string superscript_string(const boost::smatch& match)
	{
		string in = match[1];
		string out = "";
		for(int i=0; i<in.size();i++)
			out = out + code_for_superscript(in.at(i));
		return out;
	};

	string subscript_string(const boost::smatch& match)
	{
		string in = match[1];
		string out = "";
		for(int i=0; i<in.size();i++)
			out = out + code_for_subscript(in.at(i));
		return out;
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
