#ifndef KEY_HANDLER_HPP
#define KEY_HANDLER_HPP

#include "gui/window_manager.hpp"
#include "gui/screen_handler.hpp"
#include "io/file_request_handler.hpp"
#include "key_manager.hpp"
#include <ncurses.h>

class key_handler
{
	screen_handler&			sh;
	file_request_handler&	fh;
	key_manager				km;

public:
	key_handler(screen_handler& sh, file_request_handler& fh);
	bool listening();
};

#endif