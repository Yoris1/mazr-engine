#include <math.h>
#include "vectors.h"
#include "raycaster.h"
#include "sdlm.h"


#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define FOV 90

SDL_Rect pixel_row_rect;
Ray ray;
vec2d pos;

void loop(float dTime, float time, SDL_Event* event);
int main(int argc, char *argv[])
{
	pos.x = 0;
	pos.y = 0;
	if(SDLM_SetupWindowWithRenderContext("Mazr", WINDOW_WIDTH, WINDOW_HEIGHT))
		return 0;	
	SDLM_initGameLoop(&loop);
	SDLM_destroy();
	return 0;
}
#define RENDER_DIST 50

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
void loop(float dTime, float time, SDL_Event* event) {

	if(event->type == SDL_KEYDOWN) {
		if(event->key.keysym.sym == SDLK_UP)
			pos.y += 10*dTime;
		else if(event->key.keysym.sym == SDLK_DOWN)
			pos.y -= 10*dTime;
		if(event->key.keysym.sym == SDLK_RIGHT)
			pos.x += 10*dTime;
		else if(event->key.keysym.sym == SDLK_LEFT)
			pos.x -= 10*dTime;
	}
	SDL_SetRenderTarget(SDLM_renderer, SDLM_texture);

	SDL_SetRenderDrawColor(SDLM_renderer, 0x0f, 0x0f, 0xff, 0xff);
	SDL_RenderClear(SDLM_renderer);
	
	SDL_SetRenderDrawColor(SDLM_renderer, 0x0f, 0x0f, 0x0f, 0xff);
	pixel_row_rect.w = WINDOW_WIDTH;
	pixel_row_rect.x = 0;
	pixel_row_rect.y = WINDOW_HEIGHT/2;
	pixel_row_rect.h = WINDOW_HEIGHT;
	SDL_RenderFillRect(SDLM_renderer, &pixel_row_rect);
	vec2d d;
	vec2d xOffset;
	xOffset.x = 0.5f;
	xOffset.y = 0;
	pixel_row_rect.w = 1;
	
	for(int row = 0; row < WINDOW_WIDTH; row++) {
			pixel_row_rect.x = row;
			
			d.x = (float)row/WINDOW_WIDTH;
			d.y = 1; // 0.1f in front of camera
			sub(&d, &xOffset); // -0.5 to 0.5 to camera
			normalize(&d); // d is a directional vector, so must be normalized.

			ray.origin = pos;
			ray.direction = d;
			ray.dist = 0;
			if(castRay(&ray)) {
				float brightness = ray.dist*0xff/RENDER_DIST;
				brightness = 1 - brightness;
				SDL_SetRenderDrawColor(SDLM_renderer, brightness, brightness, brightness, 0xff);
				pixel_row_rect.h = pow(1-(ray.dist/RENDER_DIST), 2)*WINDOW_HEIGHT*0.5f; 
				pixel_row_rect.y = WINDOW_HEIGHT / 2 - pixel_row_rect.h / 2;
				SDL_RenderDrawRect(SDLM_renderer, &pixel_row_rect);
			}
	}
}