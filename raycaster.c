#include "raycaster.h"
#include <math.h>

char map[10][10] = {
    {1, 1, 1, 1, 2, 2, 1, 1, 1, 1},
    {1, 0, 1, 0, 0, 1, 0, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    {2, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {2, 0, 0, 0, 0, 0, 0, 0, 2, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 2, 2, 2, 1, 1, 1, 1}
};
#define TILE_SIZE 0.51f
// make this slightly bigger so i wouldn't get the lines in the middle between blocks

float getDist(vec2d* p, char* hitAxis, float* xHitPoint, char* textureId) {
	float minDist = RENDER_DIST;
    vec2d pos;
    for(int x = 0; x < 10; x++) 
    {
        for(int y = 0; y < 10; y++) {
            if(map[x][y] == 0) continue;
            pos.x = x;
            pos.y = y;
            float a, b, d;

            a = sqrt(pow(p->x - pos.x, 2))-TILE_SIZE;
            b = sqrt(pow(p->y - pos.y, 2))-TILE_SIZE;
            char tempHitAxis;
            if(a > b) {
                d = a;
                tempHitAxis = 0;
            }
            else {
                d = b;
                tempHitAxis = 1;
            }

            if(minDist > d) {
                 minDist = d;
                *hitAxis = tempHitAxis;
                if(tempHitAxis)
                    *xHitPoint = (p->x-pos.x+TILE_SIZE)/TILE_SIZE/2;
                else
                    *xHitPoint = (p->y-pos.y+TILE_SIZE)/TILE_SIZE/2;
                // xHitPoint is 0 to 1 depending on how far along the surface it is.
                *textureId = map[x][y];
            }
        }
    }

	return minDist;
}
#define HIT_DIST 0.001f
#define MAX_STEPS 100

int castRay(Ray *ray) {
	vec2d point;
	vec2d dir;
	
	
	for(int i = 0; i < MAX_STEPS; i++) {
		if(ray->dist > RENDER_DIST) 
			break;
		point.x = ray->origin.x;
		point.y = ray->origin.y;
		
		dir.x = ray->direction.x;
		dir.y = ray->direction.y;
		mulF(&dir, ray->dist);
		add(&point, &dir);

		float d = getDist(&point, &(ray->hitAxis), &(ray->hitTextureX), &(ray->textureId));

		if(d < HIT_DIST)
			return 1;

		ray->dist += d*0.9f;
	}   
	return 0;
}