#pragma once
#include "vectors.h"
#define RENDER_DIST 50

typedef struct ray {
        vec2d origin;
        vec2d direction;
        float dist;
        char hitAxis;
        float hitTextureX;
        char textureId;
} Ray;

int castRay(Ray* ray);
