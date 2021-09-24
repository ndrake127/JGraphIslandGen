#ifndef ISLAND_H
#define ISLAND_H

#include <vector>
#include <string>

class Island
{
public:
	static int count;

	Island(unsigned int x, unsigned int y);

	void SetIslandAt(int x, int y);
	bool GetIslandAt(unsigned int x, unsigned int y);
	void ReduceIslandSize();

	std::vector<std::vector<int> >* getIslandData();
	int getX();
	int getY();
	int getWidth();
	int getHeight();
	int getID();

	void setX(int x);
	void setY(int y);

	void Export(std::string filename = "island");

private:
	// stores grid of #'s indicating where in the grid the island exists.
	std::vector<std::vector<int> > data;
	
	// location of top left of rectangle containing island on initial map
	int x, maxX;
	int y, maxY;
	int width, height;

	// meta info
	int id;
	bool anySet;

	// set in constructor, stores RGB of islands
	unsigned char color[3];

};

#endif