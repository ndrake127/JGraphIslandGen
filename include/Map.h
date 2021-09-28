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

	struct stats {
		int meanIslandSize;
		int islandCount;
		float waterComposition;
	};
public:
	Map(std::string mode, std::string filename, unsigned int width, unsigned int height, int seed = -1);

	std::string getName();
	void Export();

private:
	void Generate();
	void TrimEdgeIslands();
	void ExtractIslands();
	void Finalize();
	
	void DeleteIslandFromPoint(int x, int y);
	Island* ExtractIslandFromPoint(int x, int y);

private:
	std::string mode;
	std::string filename;

	std::vector<Island*> islands;
	std::vector<std::vector<unsigned char> > noiseData;
	std::vector<std::vector<color> > map;
	stats mapStats;

	unsigned int seed;
	unsigned int width;
	unsigned int height;
	unsigned int islandCount;

	unsigned char waterColor[3];
};

#endif

