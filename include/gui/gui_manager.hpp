#ifndef GUI_MANAGER_HPP
#define GUI_MANAGER_HPP

#include <locale>
#include "io/file_request_handler.hpp"
#include "data/database_manager.hpp"
#include "gui/screen_handler.hpp"
#include "key_handling/key_handler.hpp"

class gui_manager
{
public:
	screen_handler			sh;
	key_handler				kh;

	gui_manager(database_manager& dm, file_request_handler& fh)
	: sh(dm), kh(sh, fh) {};

	void start_screen()
	{
		// std::setlocale(LC_ALL, "en_US.UTF-8");
		std::setlocale(LC_ALL, "");

		sh.initialize();
		if(! kh.listening())
			endwin();
	};
};

#endif