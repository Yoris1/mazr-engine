#include "vectors.h"
typedef struct Map Map;
typedef struct Tile Tile;
Map* loadMap(const char* fileName);

#ifndef MAP_DEFINED
#define MAP_DEFINED
typedef struct Map {
	int tile_count;
	int max_height;
	Tile* tiles;
} Map;

typedef struct Tile {
	int x;
	int y;
	char tex_id;
	char height;
}Tile;

#endif 
