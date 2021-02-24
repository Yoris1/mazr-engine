#include "map.h"
#include "stdlib.h"
#include "stdio.h"

Map* loadMap(const char* filename) {
	FILE* map_file = fopen(filename, "r");
	if(map_file == NULL)
		 // file doesn't exist
		return NULL;
	Map* map = malloc(sizeof(Map));

	map->tile_count = fgetc(map_file);
	map->max_height = fgetc(map_file);
	if(map->tile_count == EOF || map->max_height == EOF) {
		// file is pracitcally empty. 
		fclose(map_file);
		free(map); 
		return NULL;
	}

	map->tiles = malloc(sizeof(Tile)*map->tile_count);
	fread(map->tiles, sizeof(Tile), map->tile_count, map_file);

	for(int i = 0; i < map->tile_count; i++) {
		printf("|Loading map tile: %d\n|----> tex: %d, height: %d, x: %d, y:%d \n",
		i, (int)map->tiles[i].tex_id, (int)map->tiles[i].height, map->tiles[i].x, map->tiles[i].y);
	}
	fclose(map_file);
	return map;
}

void deleteMap(Map* map) {
	free(map->tiles);
	free(map);
}