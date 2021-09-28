#include "Map.h"

#include <string>
#include <iostream>
#include <unistd.h>

int main(int argc, char** argv)
{
	std::string filename = "out";
	std::string mode = "jgraph";
	int width = 1280;
	int height = 720;

	std::srand(time(NULL));
	int seed = rand();

	int c;
	while ((c = getopt(argc, argv, "f:m:w:h:s:")) != -1)
	{
		switch (c)
		{
			// set output name
		case 'f':
			filename = optarg;
			break;

			// set mode
		case 'm':
			mode = optarg;
			break;

			// set width
		case 'w':
			width = std::stoi(optarg);
			break;

			// set height
		case 'h':
			height = std::stoi(optarg);
			break;

			// set seed
		case 's':
			seed = std::stoi(optarg);
			break;
		}
	}

	Map map(mode, filename, width, height, seed);
	map.Export();
	std::string command = "./jgraph -P " + map.getName() + ".jgr | ps2pdf - | convert -density 300 - -quality 100 " + map.getName() + ".jpg";
	//std::cout << command << '\n';
	system(command.c_str());

	return 0;
}