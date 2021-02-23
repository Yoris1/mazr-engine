#include "map.h"
#include "stdlib.h"
#include "stdio.h"

Map* loadMap(const char* filename) {
	FILE* map_file = fopen(filename, "r");
	Map* map = malloc(sizeof(Map));

	map->tile_count = fgetc(map_file);
	map->max_height = fgetc(map_file);
	if(map->tile_count == EOF || map->max_height == EOF) {
		// file is pracitcally empty. 
		fclose(map_file);
		free(map); 
		return NULL;
	}
	
	map->tile_x = malloc(sizeof(char)*map->tile_count);
	map->tile_y = malloc(sizeof(char)*map->tile_count);
	map->tile_texture = malloc(sizeof(char)*map->tile_count);
	map->tile_height = malloc(sizeof(char)*map->tile_count);

	for(int i = 0; i < map->tile_count; i++) {
		map->tile_x[i] = fgetc(map_file);
		map->tile_y[i] = fgetc(map_file);
		map->tile_texture[i] = fgetc(map_file);
		map->tile_height[i] = fgetc(map_file);
	}
	fclose(map_file);
	return map;
}

void deleteMap(Map* map) {
	free(map->tile_texture);
	free(map->tile_height);
	free(map->tile_y);
	free(map->tile_x);
	free(map);
}