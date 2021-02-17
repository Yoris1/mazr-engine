#include "vectors.h"
#include <math.h>

float LALGBR_DistSq(LALGBR_Vec2d* a, LALGBR_Vec2d* b) {
    return pow(a->x-b->x, 2) + pow(a->y-b->y, 2);
}
float LALGBR_Dist(LALGBR_Vec2d* a, LALGBR_Vec2d* b) {
    return sqrtf(LALGBR_DistSq(a, b));
}

float LALGBR_SqMagnitude(LALGBR_Vec2d* a) {
    return pow(a->y, 2) + pow(a->x, 2);
}
float LALGBR_Magnitude(LALGBR_Vec2d* a) {
    return sqrtf(LALGBR_SqMagnitude(a));
}

void LALGBR_Add(LALGBR_Vec2d* a, LALGBR_Vec2d* b) {
    a->x += b->x;
    a->y += b->y;
}
void LALGBR_Sub(LALGBR_Vec2d* a, LALGBR_Vec2d* b) {
    a->x -= b->x;
    a->y -= b->y;
}
void LALGBR_SubF(LALGBR_Vec2d* a, float b) {
    a->x -= b;
    a->y -= b;
}
void LALGBR_Mul(LALGBR_Vec2d* a, LALGBR_Vec2d* b) {
    a->x *= b->x;
    a->y *= b->y;
}
void LALGBR_MulF(LALGBR_Vec2d* a, float b) {
    a->x *= b;
    a->y *= b;
}
void LALGBR_Normalize(LALGBR_Vec2d* a) {
    float mag = LALGBR_Magnitude(a);
    a->x /= mag;
    a->y /= mag;
}

LALGBR_Mat2x2 LALGBR_GetRotationMatrix(float degrees) {
    LALGBR_Mat2x2 mat;
    LALGBR_Vec2d i;
    LALGBR_Vec2d j;
    float radians = degrees * M_PI/180;
    i.x = cos(radians);
    i.y = sin(radians);
    j.x = -sin(radians);
    j.y = cos(radians);
    mat.i = i;
    mat.j = j;
    return mat;
}

void LALGBR_MulMat2x2(LALGBR_Vec2d* a, LALGBR_Mat2x2* m) {
    float nx = a->x * m->i.x + a->y * m->j.x;
    float ny = a->x * m->i.y + a->y * m->j.y;
    a->x = nx;
    a->y = ny;
}

LALGBR_Mat2x2 LALGBR_Inverse(LALGBR_Mat2x2* mat) {
    LALGBR_Mat2x2 inverse;
    float det = (mat->i.x * mat->j.y - mat->i.y * mat-> j.x);

    inverse.i.x = mat->j.y * (1.0/det);
    inverse.j.y = mat->i.x * (1.0/det);

    inverse.i.y = -mat->i.y * (1.0/det);
    inverse.j.x = -mat->j.x * (1.0/det);
    return inverse;
}