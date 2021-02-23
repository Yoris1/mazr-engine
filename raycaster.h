#pragma once
#include "map.h"
#include "vectors.h"
#include "texture_atlas.h"
#include <SDL2/SDL.h>

#define RENDER_DIST 50

typedef struct Camera Camera;

typedef struct RenderContext {
    SDL_Renderer* renderer;
    SDL_Texture* target;
    int window_width;
    int window_height;
} RenderContext;

Camera* createCamera(LALGBR_Vec2d pos, float fov);
LALGBR_Mat2x2* getCameraRotationMatrix(Camera* cam);
void setCameraPosition(LALGBR_Vec2d pos, Camera* cam);
void rotateCamera(float angle, Camera* cam);
void setCameraFov(float angle, Camera* cam);
void raycast(RenderContext* context, Camera* cam, TextureAtlas* textures, int row, Map* map);
void drawBackground(RenderContext* context);