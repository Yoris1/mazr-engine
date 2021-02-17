#include "texture_atlas.h"
TextureAtlas* loadAtlas(SDL_Renderer* renderer, const char* path, int width, int height, int countX, int countY){
    TextureAtlas* atlas = malloc(sizeof(TextureAtlas));
    SDL_Surface* surf = SDL_LoadBMP(path);
    atlas->image = SDL_CreateTextureFromSurface(renderer, surf);
    free(surf);

    if(atlas->image == NULL)
        return NULL;
    atlas->tiles_x = countX;
    atlas->tiles_y = countY;
    atlas->sampleRect.w = 1;
    atlas->sampleRect.h = height;
    atlas->sampleRect.x = 0;
    atlas->sampleRect.y = 0;
    atlas->tile_height = height;
    atlas->tile_width = width;
    return atlas;
}