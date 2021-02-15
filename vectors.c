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
void subF(vec2d* a, float b) {
    a->x -= b;
    a->y -= b;
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

mat2x2 getRotationMatrix(float degrees) {
    mat2x2 mat;
    vec2d i;
    vec2d j;
    float radians = degrees * M_PI/180;
    i.x = cos(radians);
    i.y = sin(radians);
    j.x = -sin(radians);
    j.y = cos(radians);
    mat.i = i;
    mat.j = j;
    return mat;
}

void mulMat2x2(vec2d* a, mat2x2* m) {
    float nx = a->x * m->i.x + a->y * m->j.x;
    float ny = a->x * m->i.y + a->y * m->j.y;
    a->x = nx;
    a->y = ny;
}