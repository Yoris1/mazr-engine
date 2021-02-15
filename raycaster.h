#pragma once
#include "vectors.h"
#define RENDER_DIST 50

typedef struct ray {
        vec2d origin;
        vec2d direction;
        float dist;
} Ray;

int castRay(Ray* ray);
