#ifndef FILE_REQUEST_HANDLER_HPP
#define FILE_REQUEST_HANDLER_HPP

#include "sys/stat.h"
#include <boost/regex.hpp>
#include <unistd.h>
#include <regex>
#include <string>

#include "data/data_structures.hpp"
#include "gui/status_bar.hpp"
#include "gui/screen_handler.hpp"
#include "gui/window_manager.hpp"
#include "io/xml_interface.hpp"
#include "io/tex_parser.hpp"


class file_request_handler
{
	database_manager&	dm;

public:
	file_request_handler(database_manager& dm) : dm(dm) {};
	bool save(status_bar& sb)
	{
		string filename = sb.save_dialog();
		xml_interface x(dm);
		return x.save(filename);
	};

	bool open(status_bar& sb)
	{
		string filename = sb.open_dialog();
		return open(filename);
	};

	bool open(string filename)
	{
		if(!file_exists(filename))
			return false;

		if(is_graphml(filename))
		{
			xml_interface x(dm);
			return x.open(filename);
		}
		if(is_tex(filename))
		{
			tex_parser t(dm);
			return t.open(filename);
		}
		return false;
	};

	bool is_graphml(string s)
	{
		boost::regex re("\\.graphml$");
		return boost::regex_search(s,re);
	};

	bool is_tex(string s)
	{
		boost::regex re("\\.tex$");
		return boost::regex_search(s,re);
	};

	bool file_exists(string file)
	{
	    struct stat buf;
	    return (stat(file.c_str(), &buf) == 0);
	};

	string get_working_path()
	{
		int MAXPATHLEN = 1000;
		char temp[MAXPATHLEN];
		return ( getcwd(temp, MAXPATHLEN) ? std::string( temp ) : std::string("") );
	};
};

#endif
