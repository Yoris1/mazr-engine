#include "vectors.h"
typedef struct Map Map;
Map* loadMap(const char* fileName);
#ifndef CUM
#define CUM
typedef struct Map {
	int tile_count;
	int max_height;
	char *tile_x, *tile_y;
	char *tile_texture;
	char *tile_height;
} Map;
#endif 
