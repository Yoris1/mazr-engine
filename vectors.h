#pragma once

typedef struct vec2d {
    float x;
    float y;
} vec2d;
typedef struct mat2x2 {
    vec2d i;
    vec2d j;
} mat2x2;

mat2x2 getRotationMatrix(float angle);

float dist(vec2d* a, vec2d* b);
float distSq(vec2d* a, vec2d* b);
float magnitude(vec2d* a);
float magnitudeSq(vec2d* a);

void normalize(vec2d* a);
void add(vec2d* a, vec2d* b);
void sub(vec2d* a, vec2d* b);
void subF(vec2d* a, float b);
void mul(vec2d* a, vec2d* b);
void mulF(vec2d* a, float c);
void mulMat2x2(vec2d* a, mat2x2* m);