# JGraphIslandGen

This program generates archipelagos and then graphs them using JGraph.

To run, simple run 'make'
This will handle complation and will run the program in 5 unique ways to generate 5 outputs.

To run this manually after running make, first run 'make clean' and then run as follows:

'./islandgen [args]'
Example: ./islandgen -f map4 -m jgraph -w 500 -h 500 -s 1024

Possible arguments:

-f			set the output filename. do NOT include .jpg extension. if not set, the name is "out"

-m			set the output mode, either ppm or jgraph. this program support ppm outputs, defaults to jgraph

-w			set the width of the map in pixels. if not set, the width is 1280

-h			set the height of the map in pixels. if not set, the height  is 720

-s			set the seed of the map. if not set, the seed is set randomly