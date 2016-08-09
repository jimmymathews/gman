
#include "gui/category_tile.hpp"

void category_tile::write_on(WINDOW* w, bool blinky)
{
	write_string(w, nt.get_name(), blinky);
}

void category_tile::write_on_with_size(WINDOW* w, int n, bool blinky)
{
	if(n == 1)
	{
		write_string(w, nt.get_name().substr(0,1),blinky);
	}
	else
	if(n > length())	//never used?
	{
		int lpad = (n-length())/2;
		int parity = (n-length()) % 2;
		int rpad = lpad;
		if(parity == 1)
			rpad++;
		
		write_string(w, string(lpad,' ')+nt.get_name()+string(rpad,' '), blinky);
		return;
	}
	else
	{
		write_string(w, nt.get_name().substr(0,n-1)+"~", blinky);
	}
}

void category_tile::write_string(WINDOW* w, string s, bool blinky)
{
	nt.turn_on_color(w);			//formatting
	if(is_being_edited)
		wattron(w, A_UNDERLINE);
	else
	{
		if(has_focus())
		{
			if(blinky)
				wattron(w, A_BOLD);
			else
				wattron(w, A_REVERSE);
		}
	}

	if(nt.is_enabled())				//writing
		waddstr(w, s.c_str());
	else
		waddstr(w, " ");

	if(is_being_edited)
		wattroff(w, A_UNDERLINE);
	else
	{
		if(has_focus())
		{
			if(blinky)
				wattroff(w, A_BOLD);
			else
				wattroff(w, A_REVERSE);
		}
	}

	nt.turn_off_color(w);
}

void category_tile::toggle_color()
{
	nt.toggle_color();
}