OBJS	= obj/Island.o obj/main.o obj/Map.o
SOURCE	= src/Island.cpp src/main.cpp src/Map.cpp
HEADER	= include/FastNoiseLite.h include/Island.h include/Map.h
IDIR    = include
OUT	    = islandgen
CC	    = g++
FLAGS   = -g -c -I$(IDIR) -o 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT)
	chmod u+r+x jgraph
	./islandgen
	./islandgen -f map1
	./islandgen -f map2 -w 800 -h 600
	./islandgen -f map3 -w 800 -h 600 -s 137
	./islandgen -f map4 -m jgraph -w 500 -h 500 -s 1024
	rm -f ./*.jgr

obj/Island.o: src/Island.cpp
	$(CC) $(FLAGS) obj/Island.o src/Island.cpp 

obj/main.o: src/main.cpp
	$(CC) $(FLAGS) obj/main.o src/main.cpp 

obj/Map.o: src/Map.cpp
	$(CC) $(FLAGS) obj/Map.o src/Map.cpp 

clean:
	rm -f $(OBJS) $(OUT) ./*.jgr ./*.jpg
