#include "vectors.h"
#include <math.h>

float distSq(vec2d* a, vec2d* b) {
    return pow(a->x-b->x, 2) + pow(a->y-b->y, 2);
}
float dist(vec2d* a, vec2d* b) {
    return sqrtf(distSq(a, b));
}

float magnitudeSq(vec2d* a) {
    return pow(a->y, 2) + pow(a->x, 2);
}
float magnitude(vec2d* a) {
    return sqrtf(magnitudeSq(a));
}

void add(vec2d* a, vec2d* b) {
    a->x += b->x;
    a->y += b->y;
}
void sub(vec2d* a, vec2d* b) {
    a->x -= b->x;
    a->y -= b->y;
}
void mul(vec2d* a, vec2d* b) {
    a->x *= b->x;
    a->y *= b->y;
}
void mulF(vec2d* a, float b) {
    a->x *= b;
    a->y *= b;
}
void normalize(vec2d* a) {
    float mag = magnitude(a);
    a->x /= mag;
    a->y /= mag;
}