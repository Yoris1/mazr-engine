#pragma once
#include "vectors.h"
#include <SDL2/SDL.h>
#define RENDER_DIST 50

typedef struct ray {
        LALGBR_Vec2d origin;
        LALGBR_Vec2d direction;
        float dist;
        char hitAxis;
        float hitTextureX;
        char textureId;
} Ray;
typedef struct Camera Camera;

Camera* createCamera(LALGBR_Vec2d pos, float fov);
LALGBR_Mat2x2* getCameraRotationMatrix(Camera* cam);
void setCameraPosition(LALGBR_Vec2d pos, Camera* cam);
void rotateCamera(float angle, Camera* cam);
void setCameraFov(float angle, Camera* cam);
void render(SDL_Texture *texture, SDL_Renderer *renderer,
 int window_width, int window_height,
Camera* cam, SDL_Rect* texture_sample_rect, SDL_Texture* walls_texture);
int castRay(Ray* ray);
