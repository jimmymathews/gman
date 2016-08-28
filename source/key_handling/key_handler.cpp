
#include "key_handling/key_handler.hpp"
#include <ncurses.h>

key_handler::key_handler(screen_handler& sh, file_request_handler& fh) : sh(sh), fh(fh)
{
	km.add_key("left",			"move left",						"left arrow");
	km.add_key("right",			"move right",						"right arrow");
	km.add_key("up",			"move up",							"up arrow");
	km.add_key("down",			"move down",						"down arrow");
	km.add_key("shift-left",	"move left and select",				"shift-left");
	km.add_key("shift-right",	"move right and select",			"shift-right");
	km.add_key("shift-up",		"move up and select",				"shift-up");
	km.add_key("shift-down",	"move down and select",				"shift-down");
	// km.add_key("large-left",	"move focus left",					"ctrl-left");
	// km.add_key("large-right",	"move focus right",					"ctrl-right");
	km.add_key("large-up",		"move focus up",					"ctrl-up");
	km.add_key("large-down",	"move focus down",					"ctrl-down");
	km.add_key("home",			"move to left end of line",			"home");
	km.add_key("end",			"move to right end of line", 		"end");
	km.add_key("page-up",		"move up a page",					"page up");
	km.add_key("page-down",		"move down a page",					"page down");
	km.add_key("delete",		"delete current character",			"delete");
	km.add_key("backspace",		"delete previous character and move", "backspace");
	km.add_key("ctrl-v",		"paste",							"ctrl-v");
	km.add_key("ctrl-c",		"copy",								"ctrl-c");
	km.add_key("ctrl-g",		"initiate greek input",				"ctrl-g");
	km.add_key("ctrl-e",		"toggle big editor",				"ctrl-e");
	km.add_key("ctrl-r",		"toggle relation labels",			"ctrl-r");
	km.add_key("save",			"save to file",						"ctrl-s");
	km.add_key("open",			"open file",						"ctrl-o");
	km.add_key("escape",		"quit program",						"escape-escape(twice)");
	km.startup();
}

bool key_handler::listening()
{
	while(true)
	{	
		key_datum& k = *(km.get_key(sh.wm.get_focal_window()));

		if(sh.consider_resize())
			continue;

		if (k == "save")
		{
			if(!fh.save(sh.wm.sb))
			{
				sh.wm.sb.restore_current_filename();
				sh.wm.sb.set_temporary_status("write failed; check file name");
			}
		}
		if (k == "open")
		{
			if(!fh.open(sh.wm.sb))
			{
				sh.wm.sb.restore_current_filename();
				sh.wm.sb.set_temporary_status("open failed; check file name");
			}
			sh.wm.refresh_data();
		}

		if (k == "alphanumeric")
			sh.wm.alphanumeric(k.get_code()[0]);
		else if (k == "delete")
			sh.wm.handle_delete();
		else if (k == "backspace")
			sh.wm.handle_backspace();
		else if (k == "up")	
			sh.wm.up();
		else if (k == "down")
			sh.wm.down();
		else if (k == "left")
			sh.wm.left();
		else if (k == "right")
			sh.wm.right();
		else if (k == "large-up")
			sh.wm.large_up();
		else if (k == "large-down")
			sh.wm.large_down();
		// else if (k == "large-left")
		// 	sh.wm.large_left();
		// else if (k == "large-right")
		// 	sh.wm.large_right();
		else if (k == "shift-up")
			sh.wm.shift_up();
		else if (k == "shift-down")
			sh.wm.shift_down();
		else if (k == "shift-left")
			sh.wm.shift_left();
		else if (k == "shift-right")
			sh.wm.shift_right();
		else if (k == "ctrl-v")
			sh.wm.ctrl_v();
		else if (k == "ctrl-c")
			sh.wm.ctrl_c();
		else if( k == "ctrl-g")
			sh.wm.ctrl_g();
		else if( k == "ctrl-e")
			sh.wm.ctrl_e();
		else if( k == "ctrl-r")
			sh.wm.ctrl_r();
		else if( k == "home")
			sh.wm.home();
		else if( k == "end")
			sh.wm.end();
		else if( k == "page-up")
			sh.wm.pageup();
		else if( k == "page-down")
			sh.wm.pagedown();

		if (k == "escape")
		{
			if(!sh.wm.handled_cancel())
				break;
		}

		if( k == "enter10" || k == "enter13")
			sh.wm.enter();

		sh.wm.redraw();
	}
	return false;
}