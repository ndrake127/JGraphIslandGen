#include "Map.h"
#include "FastNoiseLite.h"

#include <iostream>
#include <queue>
#include <fstream>
#include <random>
#include <algorithm>
#include <numeric>
#include <math.h>

Map::Map(std::string mode, std::string filename, unsigned int width, unsigned int height, int seed)
{
    this->mode = mode;
    this->filename = filename;
    this->width = width;
    this->height = height;
    islandCount = 0;



    if(seed != -1) this->seed = seed;

    map = std::vector<std::vector<color> >(height, std::vector<color>(width));

    waterColor[0] = 25;
    waterColor[1] = 70;
    waterColor[2] = 150;
    
    Generate();
    TrimEdgeIslands();
    ExtractIslands();
    Finalize();
}

std::string Map::getName()
{
    return filename;
}

void Map::Export()
{
    if (mode == "ppm")
    {
        std::ofstream ofs(filename + ".ppm", std::ios_base::out | std::ios_base::binary);
        ofs << "P3" << '\n' << width << ' ' << height << '\n' << "255" << '\n';

        for (unsigned int y = 0; y < height; y++)
        {
            for (unsigned int x = 0; x < width; x++)
            {
                ofs << (int)map[y][x].channel[0] << ' ' << (float)map[y][x].channel[1] << ' ' << (int)map[y][x].channel[2] << '\n';
            }
        }

        for (unsigned int i = 0; i < islands.size(); i++)
        {
            islands[i]->Export();
        }
        
        return;
    }
    
    if (mode == "jgraph")
    {
        std::ofstream ofs(filename + ".jgr", std::ios_base::out | std::ios_base::binary);
        int scaleFactor = 3;

        // draws islands in jgraph
        ofs << "newgraph" << "\n\n";

        ofs << "xaxis min 0 max " << width << " hash 0 mhash 0 size " << (float)(scaleFactor * ((float)width / (float)height)) << '\n';
        ofs << "yaxis min 0 max " << height << " hash 0 mhash 0 size " << scaleFactor << "\n\n";
        ofs << "newcurve poly pcfill 0.098 0.275 0.588 marktype none linetype solid pts "
            << "0"   << ' ' << "0 "
            << width << ' ' << "0 "
            << width << ' ' << height << ' '
            << "0"   << ' ' << height << ' '
            << "0"   << ' ' << "0 "
            << "\n\n";

        
        std::vector<std::pair<int, int> > points;
        for (unsigned int i = 0; i < islandCount; i++)
        {
            ofs << "newcurve poly pcfill 0.169 0.510 0.067 marktype none linetype solid pts " << '\n';

            points = islands[i]->getRepresentativePoints();
            for (unsigned int j = 0; j < points.size(); j++)
            {
                ofs << points[j].second + islands[i]->getX() << ' ' << height - points[j].first - islands[i]->getY() << " \n";
            }

            ofs << "\n\n";
        }

        // draws statistics in graph beneath islands
        ofs << "newgraph" << '\n'
            << "y_translate " << -1 * scaleFactor - 1 << "\n\n";

        std::vector<int> sizes;
        int maxSize = 0;

        for (unsigned int i = 0; i < islandCount; i++)
        {
            sizes.push_back(islands[i]->getSize());
            maxSize = std::max(sizes.back(), maxSize);
        }

        int maxSizeMagnitude = std::floor(log10(maxSize));
        int maxSizeRounded = (maxSize / (std::pow(10, maxSizeMagnitude))) + 1;
        maxSizeRounded = maxSizeRounded * std::pow(10, maxSizeMagnitude);

        ofs << "xaxis min 0.1 max " << (float)islandCount + 0.9 << " hash 1 mhash 0 shash 0 size " << (float)(scaleFactor * ((float)width / (float)height)) << '\n'
            << "label : Island" << "\n\n";

        ofs << "yaxis min 0 max " << maxSizeRounded << " hash " << maxSizeRounded / 10 << " mhash 0 size " << scaleFactor << '\n'
            << "label : Size of Island (pixels)" << "\n\n";

        ofs << "newstring hjl vjc x 0 y " << maxSizeRounded * 1.1 << '\n'
            << "fontsize 16 font Times-Bold : Bar Graph of Island Sizes with Mean Size" << "\n\n";
        
        // generate bar graph
        float barWidth = (float)width / (float)islandCount;
        ofs << "newcurve marktype xbar cfill 0.5 0 0 color 0.5 0 0" << '\n'
            << "marksize 0.8 50" << '\n'
            << "label : Generated Island Sizes" << "\n\n"
            << "pts\n";

        for (unsigned int i = 0; i < sizes.size(); i++)
        {
            ofs << (float)i + 1 << ' ' << sizes[i] << '\n';
        }
        ofs << '\n';

        // draw black line on x axis to redraw over bar graph
        ofs << "newline color 0 0 0 pts 0 0 " << (float)islandCount + 0.9 << " 0" << "\n\n";

        // plot mean size on bar graph
        float meanSize = (float)std::accumulate(sizes.begin(), sizes.end(), 0) / islandCount;
        
        ofs << "newcurve marktype none linetype dashed color 0 0 0 pts " << '\n'
            << "0 " << meanSize << ' '
            << (float)islandCount + 0.9 << ' ' << meanSize << "\n\n";

        ofs << "label : Mean Island Size = " << meanSize << "\n\n";


        return;
    }
}

void Map::Generate()
{
    FastNoiseLite noise;
    noise.SetSeed(seed);
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    //noise is row major, consistent with other c++ standards
    noiseData = std::vector<std::vector<unsigned char> >(height, std::vector<unsigned char>(width));

    int threshold = 255 * 10 / 16;
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
                islandCount++;
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
