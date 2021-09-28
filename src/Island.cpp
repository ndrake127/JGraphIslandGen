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
	size = 0;

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
	size++;
}

bool Island::GetIslandAt(unsigned int x, unsigned int y)
{
	return data[y][x];
}

void Island::ReduceIslandSize()
{
	// first erase from y + height to data.size()-1
	// then erase from 0 to y;
	data.erase(data.begin() + y + height + 1, data.end());
	data.erase(data.begin(), data.begin() + y - 1);

	// then erase from x + height to data[0].size()-1
	// then erase from 0 to x
	for (unsigned int j = 0; j < data.size(); j++)
	{
		data[j].erase(data[j].begin() + x + width + 1, data[j].end());
		data[j].erase(data[j].begin(), data[j].begin() + x - 1);
	}
	
	// debugging: outputs id, x, y, width, and height
	//std::cout << id << '\n' << x << ' ' << y << '\n' << width << ' ' << height << "\n\n";
}

std::vector<std::vector<int>>* Island::getIslandData()
{
	return &data;
}

std::vector<std::pair<int, int>> Island::getRepresentativePoints()
{
	std::vector<std::pair<int, int> > edgePoints;

	std::pair<int, int> firstPair = std::make_pair(0, 0);
	std::pair<int, int> currentPair = std::make_pair(0, 0);
	std::pair<int, int> checkPair = std::make_pair(0, 0);

	// this loop gets first pixel in island on top row
	for (unsigned int i = 0; i < width; i++)
	{
		if (data[1][i] == 1)
		{
			firstPair = std::make_pair(1, i);
			edgePoints.push_back(firstPair);
			
			if (i - 1 >= 0 && data[2][i - 1])
			{
				currentPair = std::make_pair(2, i - 1);
				edgePoints.push_back(currentPair);
				break;
			}

			if (data[2][i])
			{
				currentPair = std::make_pair(2, i);
				edgePoints.push_back(currentPair);
				break;
			}

			if (i + 1 >= 0 && data[2][i + 1])
			{
				currentPair = std::make_pair(2, i + 1);
				edgePoints.push_back(currentPair);
				break;
			}

			// error
		}
	}

	// continue tracing the island until the first pixel is encountered for a second time
	while (firstPair != currentPair)
	{
		//std::cout << "Island: " << id << '\n';
		//std::cout << "first pair:   " << firstPair.first << ' ' << firstPair.second << '\n';
		//std::cout << "current pair: " << currentPair.first << ' ' << currentPair.second << '\n';

		// algorithm:
		// check 8 pixels around currentPair starting at left pixel and going counterclockwise.
		// set checkPair to active adjacent pixel. if it is already land, then continue. else, if next pixel is land and checkPair is not, then next currentPair becomes pixel after checkPair

		// ALSO I UNDERSTAND how stupid this is, its really so bad soo much copy paste but i just dont think its worth the effort to refactor into functions or whatever else
		checkPair = std::make_pair(currentPair.first + 0, currentPair.second - 1);
		if (checkPair.first >= 0 && checkPair.first < height + 2 && checkPair.second >= 0 && checkPair.second < width + 2 && data[checkPair.first][checkPair.second] == 0)
		{
			//std::cout << "check pair1:   " << checkPair.first << ' ' << checkPair.second << "\n\n";
			currentPair.first++;
			currentPair.second--;
			if (currentPair.first >= 0 && currentPair.first < height + 2 && currentPair.second >= 0 && currentPair.second < width + 2 && data[currentPair.first][currentPair.second])
			{
				// success current pair is at next land
				edgePoints.push_back(currentPair);
				continue;
			}
			else
			{
				// not found, revert currentPair
				currentPair.first--;
				currentPair.second++;
			}
		}

		checkPair = std::make_pair(currentPair.first + 1, currentPair.second - 1);
		if (checkPair.first >= 0 && checkPair.first < height + 2 && checkPair.second >= 0 && checkPair.second < width + 2 && data[checkPair.first][checkPair.second] == 0)
		{
			//std::cout << "check pair2:   " << checkPair.first << ' ' << checkPair.second << "\n\n";
			currentPair.first++;
			if (currentPair.first >= 0 && currentPair.first < height + 2 && currentPair.second >= 0 && currentPair.second < width + 2 && data[currentPair.first][currentPair.second])
			{
				// success current pair is at next land
				edgePoints.push_back(currentPair);
				continue;
			}
			else
			{
				// not found, revert currentPair
				currentPair.first--;
			}
		}

		checkPair = std::make_pair(currentPair.first + 1, currentPair.second + 0);
		if (checkPair.first >= 0 && checkPair.first < height + 2 && checkPair.second >= 0 && checkPair.second < width + 2 && data[checkPair.first][checkPair.second] == 0)
		{
			//std::cout << "check pair3:   " << checkPair.first << ' ' << checkPair.second << "\n\n";
			currentPair.first++;
			currentPair.second++;
			if (currentPair.first >= 0 && currentPair.first < height + 2 && currentPair.second >= 0 && currentPair.second < width + 2 && data[currentPair.first][currentPair.second])
			{
				// success current pair is at next land
				edgePoints.push_back(currentPair);
				continue;
			}
			else
			{
				// not found, revert currentPair
				currentPair.first--;
				currentPair.second--;
			}
		}

		checkPair = std::make_pair(currentPair.first + 1, currentPair.second + 1);
		if (checkPair.first >= 0 && checkPair.first < height + 2 && checkPair.second >= 0 && checkPair.second < width + 2 && data[checkPair.first][checkPair.second] == 0)
		{
			//std::cout << "check pair4:   " << checkPair.first << ' ' << checkPair.second << "\n\n";
			currentPair.second++;
			if (currentPair.first >= 0 && currentPair.first < height + 2 && currentPair.second >= 0 && currentPair.second < width + 2 && data[currentPair.first][currentPair.second])
			{
				// success current pair is at next land
				edgePoints.push_back(currentPair);
				continue;
			}
			else
			{
				// not found, revert currentPair
				currentPair.second--;
			}
		}

		checkPair = std::make_pair(currentPair.first + 0, currentPair.second + 1);
		if (checkPair.first >= 0 && checkPair.first < height + 2 && checkPair.second >= 0 && checkPair.second < width + 2 && data[checkPair.first][checkPair.second] == 0)
		{
			//std::cout << "check pair5:   " << checkPair.first << ' ' << checkPair.second << "\n\n";
			currentPair.first--;
			currentPair.second++;
			if (currentPair.first >= 0 && currentPair.first < height + 2 && currentPair.second >= 0 && currentPair.second < width + 2 && data[currentPair.first][currentPair.second])
			{
				// success current pair is at next land
				edgePoints.push_back(currentPair);
				continue;
			}
			else
			{
				// not found, revert currentPair
				currentPair.first++;
				currentPair.second--;
			}
		}

		checkPair = std::make_pair(currentPair.first - 1, currentPair.second + 1);
		if (checkPair.first >= 0 && checkPair.first < height + 2 && checkPair.second >= 0 && checkPair.second < width + 2 && data[checkPair.first][checkPair.second] == 0)
		{
			//std::cout << "check pair6:   " << checkPair.first << ' ' << checkPair.second << "\n\n";
			currentPair.first--;
			if (currentPair.first >= 0 && currentPair.first < height + 2 && currentPair.second >= 0 && currentPair.second < width + 2 && data[currentPair.first][currentPair.second])
			{
				// success current pair is at next land
				edgePoints.push_back(currentPair);
				continue;
			}
			else
			{
				// not found, revert currentPair
				currentPair.first++;
			}
		}

		checkPair = std::make_pair(currentPair.first - 1, currentPair.second + 0);
		if (checkPair.first >= 0 && checkPair.first < height + 2 && checkPair.second >= 0 && checkPair.second < width + 2 && data[checkPair.first][checkPair.second] == 0)
		{
			//std::cout << "check pair7:   " << checkPair.first << ' ' << checkPair.second << "\n\n";
			currentPair.first--;
			currentPair.second--;
			if (currentPair.first >= 0 && currentPair.first < height + 2 && currentPair.second >= 0 && currentPair.second < width + 2 && data[currentPair.first][currentPair.second])
			{
				// success current pair is at next land
				edgePoints.push_back(currentPair);
				continue;
			}
			else
			{
				// not found, revert currentPair
				currentPair.first++;
				currentPair.second++;
			}
		}

		checkPair = std::make_pair(currentPair.first - 1, currentPair.second - 1);
		if (checkPair.first >= 0 && checkPair.first < height + 2 && checkPair.second >= 0 && checkPair.second < width + 2 && data[checkPair.first][checkPair.second] == 0)
		{
			//std::cout << "check pair8:   " << checkPair.first << ' ' << checkPair.second << "\n\n";
			currentPair.second--;
			if (currentPair.first >= 0 && currentPair.first < height + 2 && currentPair.second >= 0 && currentPair.second < width + 2 && data[currentPair.first][currentPair.second])
			{
				// success current pair is at next land
				edgePoints.push_back(currentPair);
				continue;
			}
			else
			{
				// not found, revert currentPair
				currentPair.second++;
			}
		}
	}

	/*
	for (unsigned int i = 0; i < edgePoints.size(); i++)
	{
		edgePoints[i].first--;
		edgePoints[i].second--;
		std::cout << edgePoints[i].second << ' ' << edgePoints[i].first << '\n';
	}
	std::cout << '\n';
	int a;
	std::cin >> a;
	*/

	representativePoints = edgePoints;
	return representativePoints;
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

int Island::getSize()
{
	return size;
}

void Island::setX(int x)
{
	this->x = x;
}

void Island::setY(int y)
{
	this->y = y;
}

// this exports ppm with island
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

