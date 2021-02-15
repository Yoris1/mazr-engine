#include "raycaster.h"
#include <math.h>

vec2d cubePositions[3];
float cubeSizes[3] = {2, 1.5, 0.5};
float getDist(vec2d* p) {
	cubePositions[0].x = 0;
	cubePositions[0].y = 15;
	
	cubePositions[1].x = 3;
	cubePositions[1].y = 20;
	
	cubePositions[2].x = -1;
	cubePositions[2].y = 10;
	float minDist = RENDER_DIST;
	for(int i = 0; i < 3; i++) {
		float a, b, d;

		a = sqrt(pow(p->x - cubePositions[i].x, 2))-cubeSizes[i];
		b = sqrt(pow(p->y - cubePositions[i].y, 2))-cubeSizes[i];
		d = a>b? a:b;
		//float d = dist(p, &cubePositions[i])-1;
		// this equation is actually for a cylinder oops.
		if(minDist > d)
			minDist = d;
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

		float d = getDist(&point);
		if(d < HIT_DIST)
			return 1;
		ray->dist += d*0.9f;
	}
	return 0;
}