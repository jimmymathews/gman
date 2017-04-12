
#include "gat.hpp"
#include <string>
#include <locale.h>

int main(int argc, char** argv)
{
	// std::setlocale(LC_ALL, "en_US.UTF-8");
	std::setlocale(LC_ALL, "");//??
	gat g;
	std::string filename = std::string("");
	if(argc > 1)
	{
		filename= std::string(argv[1]);
		g.fh.open(filename);
	}
	g.gm.sh.wm.sb.first_time_opened(filename);
	g.gm.sh.wm.refresh_data();
	g.open_editor();
	return 0;
}
