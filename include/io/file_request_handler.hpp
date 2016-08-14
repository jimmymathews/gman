#ifndef FILE_REQUEST_HANDLER_HPP
#define FILE_REQUEST_HANDLER_HPP

#include <unistd.h>

#include "data/data_structures.hpp"
#include "gui/status_bar.hpp"
#include "gui/screen_handler.hpp"
#include "gui/window_manager.hpp"
#include "io/xml_interface.hpp"

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
		xml_interface x(dm);
		return x.open(filename);
	};

	string get_working_path()
	{
		int MAXPATHLEN = 1000;
		char temp[MAXPATHLEN];
		return ( getcwd(temp, MAXPATHLEN) ? std::string( temp ) : std::string("") );
	};
};

#endif
