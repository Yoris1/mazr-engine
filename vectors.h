#pragma once

typedef struct LALGBR_Vec2d {
    float x;
    float y;
} LALGBR_Vec2d;
typedef struct LALGBR_Mat2x2 {
    LALGBR_Vec2d i;
    LALGBR_Vec2d j;
} LALGBR_Mat2x2;

LALGBR_Mat2x2 LALGBR_GetRotationMatrix(float angle);
LALGBR_Mat2x2 LALGBR_Inverse(LALGBR_Mat2x2* mat);

float LALGBR_Dist(LALGBR_Vec2d* a, LALGBR_Vec2d* b);
float LALGBR_DistSq(LALGBR_Vec2d* a, LALGBR_Vec2d* b);
float LALGBR_Magnitude(LALGBR_Vec2d* a);
float LALGBR_SqMagnitude(LALGBR_Vec2d* a);

void LALGBR_Normalize(LALGBR_Vec2d* a);
void LALGBR_Add(LALGBR_Vec2d* a, LALGBR_Vec2d* b);
void LALGBR_Sub(LALGBR_Vec2d* a, LALGBR_Vec2d* b);
void LALGBR_SubF(LALGBR_Vec2d* a, float b);
void LALGBR_Mul(LALGBR_Vec2d* a, LALGBR_Vec2d* b);
void LALGBR_MulF(LALGBR_Vec2d* a, float c);
void LALGBR_MulMat2x2(LALGBR_Vec2d* a, LALGBR_Mat2x2* m);