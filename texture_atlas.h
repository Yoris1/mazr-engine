#pragma once
#include <SDL2/SDL.h>
typedef struct TextureAtlas {
    SDL_Rect sampleRect;
    SDL_Texture* image;
    int tiles_x;
    int tiles_y;
    int tile_height;
    int tile_width;
} TextureAtlas;
TextureAtlas* loadAtlas(SDL_Renderer* renderer, const char* path, int width, int height, int countX, int countY);
void destroyAtlas(TextureAtlas* atlas);