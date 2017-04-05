
#include "gat.hpp"
#include <string>

int main(int argc, char** argv)
{
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
