#ifndef MAP_H
#define MAP_H

#include "Island.h"

#include <vector>
#include <string>

class Map
{
private:
	struct color {
		unsigned char channel[3];
	};
public:
	Map(unsigned int width = 800, unsigned int height = 600, unsigned int maxIslandCount = 8, unsigned int seed = 127);

	void Export(std::string filename = "out.ppm");

private:
	void Generate();
	void TrimEdgeIslands();
	void ExtractIslands();
	void Finalize();
	
	void DeleteIslandFromPoint(int x, int y);
	Island* ExtractIslandFromPoint(int x, int y);

private:
	std::vector<Island*> islands;
	std::vector<std::vector<unsigned char> > noiseData;
	std::vector<std::vector<color> > map;

	unsigned int seed;
	unsigned int width;
	unsigned int height;
	unsigned int islandCount;

	unsigned char waterColor[3];
};

#endif

