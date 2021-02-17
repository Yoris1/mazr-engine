#pragma once
#include "vectors.h"
#define RENDER_DIST 50

typedef struct ray {
        LALGBR_Vec2d origin;
        LALGBR_Vec2d direction;
        float dist;
        char hitAxis;
        float hitTextureX;
        char textureId;
} Ray;

int castRay(Ray* ray);
