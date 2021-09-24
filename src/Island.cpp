#include "Island.h"

#include <iostream>
#include <fstream>
#include <algorithm>

int Island::count = 0;

Island::Island(unsigned int x, unsigned int y)
{
	data = std::vector<std::vector<int> >(y, std::vector<int>(x, 0));

	for (unsigned int j = 0; j < y; j++)
	{
		for (unsigned int i = 0; i < x; i++)
		{
			data[j][i] = 0;
		}
	}

	x = 0;
	maxX = 0;
	y = 0;
	maxY = 0;
	width = 0;
	height = 0;

	color[0] = 43;
	color[1] = 130;
	color[2] = 17;

	id = count++;
	anySet = false;
}

void Island::SetIslandAt(int x, int y)
{
	// sets x and y to first known point on island, because 0 is not a good initial value
	if (anySet == false)
	{
		this->x = x;
		this->y = y;
		anySet = true;
	}

	// these track the position of the upper left of the island
	this->x = std::min(x, this->x);
	maxX    = std::max(x, this->maxX);
	this->y = std::min(y, this->y);
	maxY    = std::max(y, this->maxY);

	// these track the width and height of the island
	width = maxX - this->x + 1;
	height = maxY - this->y + 1;

	data[y][x] = 1;
}

bool Island::GetIslandAt(unsigned int x, unsigned int y)
{
	return data[y][x];
}

void Island::ReduceIslandSize()
{
	// first erase from y + height to data.size()-1
	// then erase from 0 to y;
	data.erase(data.begin() + y + height, data.end());
	data.erase(data.begin(), data.begin() + y);

	// then erase from x + height to data[0].size()-1
	// then erase from 0 to x
	for (unsigned int j = 0; j < data.size(); j++)
	{
		data[j].erase(data[j].begin() + x + width, data[j].end());
		data[j].erase(data[j].begin(), data[j].begin() + x);
	}
	
	// debugging: outputs id, x, y, width, and height
	//std::cout << id << '\n' << x << ' ' << y << '\n' << width << ' ' << height << "\n\n";
}

std::vector<std::vector<int>>* Island::getIslandData()
{
	return &data;
}

int Island::getX()
{
	return x;
}

int Island::getY()
{
	return y;
}

int Island::getWidth()
{
	return width;
}

int Island::getHeight()
{
	return height;
}

int Island::getID()
{
	return id;
}

void Island::setX(int x)
{
	this->x = x;
}

void Island::setY(int y)
{
	this->y = y;
}

void Island::Export(std::string filename)
{
	std::ofstream ofs(filename + std::to_string(id) + ".ppm", std::ios_base::out | std::ios_base::binary);
	ofs << "P3" << '\n' << data[0].size() << ' ' << data.size() << '\n' << "255" << '\n';

	for (unsigned int j = 0; j < data.size(); j++)
	{
		for (unsigned int i = 0; i < data[0].size(); i++)
		{
			if (data[j][i] == 1)
			{
				ofs << (int)color[0] << ' ' << (int)color[1] << ' ' << (int)color[2] << '\n';
				continue;
			}
			else
			{
				ofs << 0 << ' ' << 0 << ' ' << 0 << '\n';
			}
		}
	}
}

