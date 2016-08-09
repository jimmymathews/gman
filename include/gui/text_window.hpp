#ifndef TEXT_WINDOW_HPP
#define TEXT_WINDOW_HPP

#include <ncurses.h>

class text_window
{
protected:
	WINDOW* w;
	int y;
	int x;
	int starty;
	int startx;
	bool active = true;
	bool has_focus = false;
	virtual void make(int termy, int termx)=0;

public:
	virtual void	initialize(int termy, int termx)=0;
	void			resize(int termy, int termx)
	{delwin(window());	make(termy,termx);};
	virtual	void	draw()=0;
	WINDOW*			window() {return w;};

	virtual void 	lose_focus()	{has_focus = false;};
	virtual void	get_focus()		{has_focus = true; };
	void			set_activity(bool a)	{active = a;}
	bool			is_active()			{return active;};
	~text_window() {delwin(w);};
};

#endif