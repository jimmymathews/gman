#include "key_manager.hpp"
#include <string>

int main(int argc, char** argv)
{
	key_manager km;				//optional: key_manager km("a complete file name for storing the key codes");
	km.add_key(mk_left,	"left",	"move left",	"left arrow");	// your key code name, an internal name, description, and description of the default keys the user should try
	km.add_key(mk_right,"right","move right",	"right arrow");
	km.startup();				//gets and stores key codes from user if not defined in the configuration file
	km.startup_ncurses();

	addstr("Type q to exit.");
	while(true)
	{
		key_datum& k = *(km.get_key());
		if(k == mk_alphanumeric)
		{
			int code = k.get_code()[0];
			addstr( ("You typed an alphanumeric key with code "+ to_string(code)+".\n").c_str() );
			if(code == 'q')
				break;
		}
		else
		if(k == mk_left)
			addstr("You typed the left arrow key.\n");
		else
			addstr("You typed a different key.\n");
	}
	endwin();
	return 0;
}