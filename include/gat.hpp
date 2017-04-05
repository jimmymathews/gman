#ifndef GAT_HPP
#define GAT_HPP

#include "data/database_manager.hpp"
#include "gui/gui_manager.hpp"

class gat //Why is it still called gat?
{
public:
	database_manager		dm;
	file_request_handler 	fh;
	gui_manager				gm;

	gat() : fh(dm), gm(dm, fh) {};
	void open_editor()
	{
		gm.start_screen();
	};
};

#endif