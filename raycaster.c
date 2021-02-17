#include "raycaster.h"
#include "texture_atlas.h"
#include <math.h>
#include <stdlib.h>

typedef struct ray {
        LALGBR_Vec2d origin;
        LALGBR_Vec2d direction;
        float dist;
        char hitAxis;
        float uv;
        char textureId;
} Ray;

typedef struct Camera  {
    LALGBR_Vec2d pos;
    float rotation;
    LALGBR_Mat2x2 rotationMatrix;

    float fovAngle;
    float fov;
} Camera;

void recreateCameraRotationMatrix(Camera* cam) {
    cam->rotationMatrix = LALGBR_GetRotationMatrix(cam->rotation);
}
Camera* createCamera(LALGBR_Vec2d pos, float fov) {
    Camera* cam = (Camera*) malloc(sizeof(Camera));
    cam->pos = pos;
    cam->rotation = 0;
    recreateCameraRotationMatrix(cam);
    setCameraFov(fov, cam);
    return cam;
}
LALGBR_Mat2x2* getCameraRotationMatrix(Camera* cam) {
    return &(cam->rotationMatrix);
}
void setCameraPosition(LALGBR_Vec2d pos, Camera* cam){
    cam->pos = pos;
}
void rotateCamera(float angle, Camera* cam) {
    cam->rotation += angle;
    recreateCameraRotationMatrix(cam);
}
void setCameraFov(float angle, Camera* cam) {
    cam->fovAngle = angle;
    cam->fov = atan(M_PI/180*(cam->fovAngle/2))*2;
}



#define TILE_SIZE 0.51f
// Tile is slightly bigger than grid so there wouldn't be any empty columns between them
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

float getDist(LALGBR_Vec2d* p, char* hitAxis, float* uv, char* textureId) {
	float minDist = RENDER_DIST;
    LALGBR_Vec2d pos;
    
    float a, b, d;
    for(int x = 0; x < 10; x++) 
    {
        for(int y = 0; y < 10; y++) {
            if(map[x][y] == 0) continue;
            pos.x = x;
            pos.y = y;

            a = p->x - pos.x;
            a*=a;
            b = p->y - pos.y;
            b*=b;

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
                    *uv = (p->x-pos.x+TILE_SIZE)/TILE_SIZE/2;
                else
                    *uv = (p->y-pos.y+TILE_SIZE)/TILE_SIZE/2;
                // uv is 0 to 1.
                *textureId = map[x][y];
            }
        }
    }
	return SDL_sqrtf(minDist)-TILE_SIZE;
}

#define HIT_DIST 0.005f
#define MAX_STEPS 100

int testRay(Ray *ray) {
	LALGBR_Vec2d point;
	LALGBR_Vec2d dir;
	
	
	for(int i = 0; i < MAX_STEPS; i++) {
		if(ray->dist > RENDER_DIST) 
			break;
		point.x = ray->origin.x;
		point.y = ray->origin.y;
		
		dir.x = ray->direction.x;
		dir.y = ray->direction.y;
		LALGBR_MulF(&dir, ray->dist);
		LALGBR_Add(&point, &dir);

		float d = getDist(&point, &(ray->hitAxis), &(ray->uv), &(ray->textureId));

		if(d < HIT_DIST)
			return 1;

		ray->dist += d*0.9f;
	}   
	return 0;
}

SDL_Rect pixel_column_rect;

void raycast(RenderContext* context, Camera* cam, TextureAtlas* textures) {
    SDL_SetRenderDrawBlendMode(context->renderer, SDL_BLENDMODE_MUL);
	
    SDL_SetRenderTarget(context->renderer, context->target);

    SDL_SetRenderDrawColor(context->renderer, 0x0f, 0x0f, 0xff, 0xff);
	SDL_RenderClear(context->renderer);
	
	SDL_SetRenderDrawColor(context->renderer, 0x0f, 0x0f, 0x0f, 0xff);
	pixel_column_rect.w = context->window_width;
	pixel_column_rect.x = 0;

	pixel_column_rect.y = context->window_height/2;
	pixel_column_rect.h = context->window_height;
	SDL_RenderFillRect(context->renderer, &pixel_column_rect);
    Ray ray;

	pixel_column_rect.w = 1;
	
	for(int column = 0; column < context->window_width; column++) {
		pixel_column_rect.x = column;
		
		ray.direction.x = (float)column/context->window_width;
		ray.direction.x -= 0.5f;
		ray.direction.y = 1.0f;
		ray.direction.x *= context->window_width/context->window_height;
		ray.direction.x *= cam->fov;

		LALGBR_Normalize(&ray.direction);
		LALGBR_MulMat2x2(&ray.direction, &cam->rotationMatrix);

		ray.origin = cam->pos;
		ray.dist = 0;
		ray.hitAxis = 0;
		if(testRay(&ray)) {
			pixel_column_rect.h = context->window_height/ray.dist;

			pixel_column_rect.y = context->window_height / 2 - pixel_column_rect.h / 2;



            int textureColumn = (ray.textureId-1)%(textures->tiles_x);
            int textureRow = ((ray.textureId-1)-textureColumn)/textures->tiles_y;
			
            //textures->sampleRect.w = 1;
            //textures->sampleRect.h = textures->tile_height;

            textures->sampleRect.x = 
            floor(ray.uv*textures->tile_width+textures->tile_width*textureColumn);
            textures->sampleRect.y = floor(textureRow*32);

			SDL_RenderCopy(context->renderer, textures->image, &textures->sampleRect, &pixel_column_rect);
			

			// SDL_SetRenderDrawColor(SDLM_renderer, ray.uv*255*(ray.textureId==2?0:1), ray.uv*255*(ray.textureId==1?0:1), 0, 0x0f);
			if(ray.hitAxis){
				SDL_SetRenderDrawColor(context->renderer, 70, 70, 70, 0x70);
				SDL_RenderDrawRect(context->renderer, &pixel_column_rect);
			}
		}
	}
}