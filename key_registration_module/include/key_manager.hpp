#ifndef KEY_MANAGER_HPP
#define KEY_MANAGER_HPP

#include "GetPot.h"
#include <ncurses.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

class key_datum
{
	string		name;
	string		description;
	string		default_keys_description;
	vector<int> codes;
	bool		lock_codes;
public:
	key_datum(string name, string description, string default_keys_description)
	:	name(name),
		description(description),
		default_keys_description(default_keys_description),
		lock_codes(false)	{};
	key_datum(string name, vector<int> sequence)
	:	name(name),
		description(""),
		default_keys_description(""),
		codes(sequence),
		lock_codes(true)	{};

	void extract_code (GetPot& o)
	{
		if(!lock_codes)
		{
			codes.clear();
			const char* cname = name.c_str();
			for(int i=0; i<o.vector_variable_size(cname); i++)
			{
				int var = o(cname,-1, i);
				codes.push_back(var);
			}
		}
	};

	void obtain_code_from_user()
	{
		if(!lock_codes)
		{
			addstr( ("[" + default_keys_description + "] "+ description +": ").c_str() );
			get_sequence();
			addstr( (display_sequence()+"\n").c_str() );
		}
	};

	void get_sequence()
	{
		codes.clear();
		int ch = getch();
		if(is_alphanumeric(ch))
		{
			codes.push_back(ch);
			return;
		}
		else
		do{	codes.push_back(ch); }
		while( (ch=getch()) != 10 && (ch) != 13);
	};

	bool is_alphanumeric		(int ch){ return (32 <= ch && ch <= 126); };
	
	string display_sequence		()
	{
		string display;
		for(int i=0; i<codes.size(); i++)
			display = display + to_string(codes[i]) + " ";
		return display;
	};

	void save_code				(ofstream& ofs)
	{
		ofs << name << " = \'";
		for(int i=0; i<codes.size(); i++)
	    	ofs << " " << codes[i];

	    ofs << " \'\n";
	};

	bool matches(key_datum& other)
	{
		vector<int>& other_code = other.get_code();
		if(other_code.size() != codes.size())
			return false;
		for(int i=0; i<other_code.size(); i++)
		{
			if( other_code[i] != codes[i] )
				return false;
		}
		return true;
	};

	vector<int>& get_code		(){return codes;};
	void set_code				(vector<int> c){codes = c;};
	void clear					(){codes.clear();};
	void set_name			(string n) {name = n;};
	void add_character			(int ch) {codes.push_back(ch);};
	string to_single_character	()
	{
		if(codes.size() == 1 && is_alphanumeric(codes[0]))
		{
			return to_string(char(codes[0]));
		}
		else
			return "Not a single character.";
	};
	bool operator==(string other){return name==other;};
};

/*********************************************************************/
/*********************************************************************/

class key_data
{
	vector<key_datum>	key_list;
public:
	key_datum			scratch_key;
	key_data() : scratch_key("","",""){};
	void	build_known_key_list()
	{
		vector<int> sequence;

		sequence.clear();	sequence.push_back(13);
		add_forced_key("enter13", sequence);
		
		sequence.clear();	sequence.push_back(10);
		add_forced_key("enter10", sequence);
	}
	void	add_key(string name, string description, string default_keys_description)
	{
		key_datum kdm(name, description, default_keys_description);
		key_list.push_back(kdm);
	}
	void 	add_forced_key(string name, vector<int> sequence)
	{
		key_datum kdm(name, sequence);
		key_list.push_back(kdm);
	}
	vector<key_datum>& keys()
	{
		return key_list;
	}
	key_datum* match(key_datum& k)
	{
		for(int i=0; i<key_list.size(); i++)
		{
			if(key_list[i].matches(k))
			{
				return &(key_list[i]);
			}
		}
		return NULL;
	}
};

/*********************************************************************/
/*********************************************************************/

class key_registerer
{
	GetPot*				op;
	key_data&	kd;
public:
	key_registerer(key_data& kd) : kd(kd) {};
	
	void initialize(string filename)
	{
		kd.build_known_key_list();
		if(file_exists(filename))
		{
			op = new GetPot(filename.c_str());
			parse_from_file(*op);
		}
		else
		{
			get_key_list_from_user();
			write_key_list_to_file(filename);
		}
	};

	void initialize() {	initialize(home_directory() + ".mk_keys"); };
	bool file_exists(string name)
	{ 
		ifstream f(name.c_str());
	    if (f.good())
	    {
	        f.close();
	        return true;
	    } else
	    {
	        f.close();
	        return false;
	    }
	};

	string home_directory()
	{
		string dir = getenv("HOME");
		return dir + "/";
	};

	void parse_from_file(GetPot& o)
	{
		for(int i=0; i<kd.keys().size();i++)
			kd.keys()[i].extract_code(o);
	};

	void get_key_list_from_user()
	{
		initscr();
		noecho();
		raw();
		refresh();
		scrollok(stdscr,true);
		addstr("Choose keys for the following actions.\nIf you wish to use the defaults in brackets, type them.\nYou will need to press enter after key combinations passed on by the terminal emulator as a sequence of characters.\nThis step is necessary because different terminal emulators pass on different key codes.\nIf you make a mistake, delete .mk_keys from your home folder.\n");

		for(int i=0; i<kd.keys().size(); i++)
			kd.keys()[i].obtain_code_from_user();	

		addstr("\nPress any key to continue.");
		getch();
	};

	void write_key_list_to_file(string filename)
	{
		ofstream ofs(filename.c_str());
	    if (ofs.good())
	    {
	    	for(int i=0; i<kd.keys().size();i++)
	    		kd.keys()[i].save_code(ofs);
	    	ofs.close();
		}
		else
			addstr( ("File writing on " + filename + " didn't work").c_str() );
		noraw();
		endwin();
	};

	void startup_ncurses()
	{
		initscr();
		cbreak();
		raw();
		noecho();
		clear();
		refresh();
	};
};

/*********************************************************************/
/*********************************************************************/

class key_receiver
{
	key_data&			kd;
	vector<key_datum>&	key_list;
public:
	key_receiver(key_data& kd) : kd(kd), key_list(kd.keys()) {};
	key_datum*	get_key(WINDOW* dummy)
	{
		//To be used in ncurses ordinary cbreak delayed, blocking mode.
		int ch = mvwgetch(dummy, 0, 0);		//Get rid of the "mv" part?
		if(is_alphanumeric(ch))
			return alphanumeric_key(ch);
	
		key_datum temporary_key("","","");
		temporary_key.add_character(ch);
		key_datum* key_pointer = NULL;
		do
		{
			key_pointer = kd.match(temporary_key);	//Still needs: Abort on fail to match any!
			if(key_pointer!= NULL)
				return key_pointer;
			ch = mvwgetch(dummy, 0, 0);				//Get rid of the "mv" part?
			temporary_key.add_character(ch);
			if(temporary_key.get_code().size() > 10)
				break;								// Log error?
		}while(key_pointer == NULL);
		
		kd.scratch_key.clear();
		return &(kd.scratch_key);
	};
	
	bool 		is_alphanumeric(int ch) { return (32 <= ch && ch <= 126); };
	key_datum*	alphanumeric_key(int ch)
	{
		kd.scratch_key.clear();
		kd.scratch_key.set_name("alphanumeric");
		kd.scratch_key.add_character(ch);
		return &kd.scratch_key;
	};
};

/*********************************************************************/
/*********************************************************************/

class key_manager
{
	key_data	kd;
	key_registerer	kreg;
	key_receiver	kr;
	string			filename;
public:
	key_manager()				: kreg(kd), kr(kd), filename("") {};
	key_manager(string filename): kreg(kd), kr(kd), filename(filename) {};
	void add_key(string name, string description, string default_keys)
	{ kd.add_key(name, description, default_keys);};
	key_datum* get_key()			{return get_key(stdscr);};
	key_datum* get_key(WINDOW* w)
	{
		return kr.get_key(w);
	};
	void startup()
	{
		if(filename=="")
			kreg.initialize();
		else
			kreg.initialize(filename);
	};
	void startup_ncurses() { kreg.startup_ncurses();};
};

#endif