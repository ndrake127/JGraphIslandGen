#include "Map.h"
#include "FastNoiseLite.h"

#include <iostream>
#include <queue>
#include <fstream>
#include <random>

Map::Map(unsigned int width, unsigned int height, unsigned int maxIslandCount, unsigned int seed)
{
    this->width = width;
    this->height = height;
    islandCount = maxIslandCount;

    std::srand(time(NULL));
    this->seed = rand();

    map = std::vector<std::vector<color> >(height, std::vector<color>(width));

    waterColor[0] = 25;
    waterColor[1] = 70;
    waterColor[2] = 150;
    
    Generate();
    TrimEdgeIslands();
    ExtractIslands();
    Finalize();
}

void Map::Export(std::string filename)
{
    std::ofstream ofs(filename, std::ios_base::out | std::ios_base::binary);
    ofs << "P3" << '\n' << width << ' ' << height << '\n' << "255" << '\n';

    for (unsigned int y = 0; y < height; y++)
    {
        for (unsigned int x = 0; x < width; x++)
        {
            ofs << (int)map[y][x].channel[0] << ' ' << (int)map[y][x].channel[1] << ' ' << (int)map[y][x].channel[2] << '\n';
        }
    }
}

void Map::Generate()
{
    FastNoiseLite noise;
    noise.SetSeed(seed);
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    //noise is row major, consistent with other c++ standards
    noiseData = std::vector<std::vector<unsigned char> >(height, std::vector<unsigned char>(width));

    int threshold = 255 * 9 / 16;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            // gets noise and changes range from (-1, 1) to (0, 255)
            noiseData[y][x] = 255 * ((noise.GetNoise((float)x, (float)y) + 1) / 2);

            if (noiseData[y][x] > threshold)
                noiseData[y][x] = 255;
            else
                noiseData[y][x] = 0;
        }
    }
}

void Map::TrimEdgeIslands()
{
    // storing bounds in arrays so i can refer to bounds by index in a for loop to reduce function calls
    unsigned int xBounds[2] = { 0, width  - 1 };
    unsigned int yBounds[2] = { 0, height - 1 };

    for (unsigned int x = 0; x < width; x++)
    {
        for (unsigned int i = 0; i < 2; i++)
        {
            if (noiseData[yBounds[i]][x] == 255)
            {
                DeleteIslandFromPoint(x, yBounds[i]);
            }
        }
    }
    for (unsigned int y = 1; y < height - 1; y++)
    {
        for (unsigned int i = 0; i < 2; i++)
        {
            if (noiseData[y][xBounds[i]] == 255)
            {
                DeleteIslandFromPoint(xBounds[i], y);
            }
        }
    }
}

void Map::DeleteIslandFromPoint(int x, int y)
{
    std::queue<std::pair<int, int> > connected;
    connected.push(std::make_pair(x, y));

    while (connected.empty() == false)
    {
        // extract current pixel
        x = connected.front().first;
        y = connected.front().second;
        connected.pop();

        // don't operate on already processed pixels
        if (noiseData[y][x] == 0)
            continue;

        // update color at current pixel
        noiseData[y][x] = 0;

        // add all adjacent white pixels to list
        if (x - 1 >= 0)
        {
            if (noiseData[y][x - 1] == 255)
            {
                connected.push(std::make_pair(x - 1, y));
            }
        }
        if (x + 1 < width)
        {
            if (noiseData[y][x + 1] == 255)
            {
                connected.push(std::make_pair(x + 1, y));
            }
        }
        if (y - 1 >= 0)
        {
            if (noiseData[y - 1][x] == 255)
            {
                connected.push(std::make_pair(x, y - 1));
            }
        }
        if (y + 1 < height)
        {
            if (noiseData[y + 1][x] == 255)
            {
                connected.push(std::make_pair(x, y + 1));
            }
        }
    }
}

void Map::ExtractIslands()
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (noiseData[y][x] == 255)
            {
                islands.push_back(ExtractIslandFromPoint(x, y));
                islands.back()->Export();
            }
        }
    }
}

void Map::Finalize()
{
    color blue  = { 20, 70, 150 };
    color green = { 40, 130, 20 };

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            map[y][x] = blue;
        }
    }

    // pastes extracted islands back onto the map with included modifications, including transforms
    std::vector<std::vector<int> > *data;
    for (unsigned int i = 0; i < islands.size(); i++)
    {
        data = islands[i]->getIslandData();

        for (int y = islands[i]->getY(); y < islands[i]->getHeight() + islands[i]->getY(); y++)
        {
            for (int x = islands[i]->getX(); x < islands[i]->getWidth() + islands[i]->getX(); x++)
            {
                if ((*data)[y - islands[i]->getY()][x - islands[i]->getX()])
                {
                    map[y][x] = green;
                }

            }
        }
    }
}

Island* Map::ExtractIslandFromPoint(int x, int y)
{
    Island* island = new Island(width, height);

    std::queue<std::pair<int, int> > connected;
    connected.push(std::make_pair(x, y));

    while (connected.empty() == false)
    {
        // extract current pixel
        x = connected.front().first;
        y = connected.front().second;
        connected.pop();

        // don't operate on already processed pixels
        if (noiseData[y][x] == 255)
        {
            noiseData[y][x] = 0;
            island->SetIslandAt(x, y);
        }
        else
        {
            continue;
        }

        // add all adjacent white pixels to list
        if (x > 0)
        {
            if (noiseData[y][x - 1] == 255)
            {
                connected.push(std::make_pair(x - 1, y));
            }
        }
        if (x < width - 1)
        {
            if (noiseData[y][x + 1] == 255)
            {
                connected.push(std::make_pair(x + 1, y));
            }
        }
        if (y > 0)
        {
            if (noiseData[y - 1][x] == 255)
            {
                connected.push(std::make_pair(x, y - 1));
            }
        }
        if (y < height - 1)
        {
            if (noiseData[y + 1][x] == 255)
            {
                connected.push(std::make_pair(x, y + 1));
            }
        }
    }

    island->ReduceIslandSize();
    return island;
}
