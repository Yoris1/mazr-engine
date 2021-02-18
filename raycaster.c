#include "raycaster.h"
#include "texture_atlas.h"
#include <math.h>
#include <stdlib.h>

typedef struct ray {
        LALGBR_Vec2d origin;
        LALGBR_Vec2d direction;
} Ray;
typedef struct ray_hit {
    LALGBR_Vec2d point;
    LALGBR_Vec2d tile_pos;
    char textureId;
    float dist;
    float uv;
    char hit_face; // [0; 4)
} Hit;

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



#define TILE_SIZE 0.5f
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

float getDist(LALGBR_Vec2d* p, Hit* hit) {
	float minDist = RENDER_DIST;
    
    float a, b, d;
    for(int x = 0; x < 10; x++) 
    {
        for(int y = 0; y < 10; y++) {
            if(map[x][y] == 0) continue;
            a = fabsf(p->x - x);
            b = fabsf(p->y - y);

            if(a > b)
                d = a;
            else
                d = b;

            if(minDist > d) {
                 minDist = d;
                hit->tile_pos.x = x;
                hit->tile_pos.y = y;
                hit->textureId = map[x][y];
            }
        }
    }
	return minDist-TILE_SIZE;
}
void calculateHitUVAndFace(Hit* hit) {
    float a = hit->point.x - hit->tile_pos.x;
    float b = hit->point.y - hit->tile_pos.y;
    if(a*a > b*b) {
        if(hit->point.x > hit->tile_pos.x)
            hit->hit_face = 0;
        else
            hit->hit_face = 2;
        hit->uv = (b+TILE_SIZE)/2/TILE_SIZE;
    } else {
        if(hit->point.y > hit->tile_pos.y)
            hit->hit_face = 1;
        else
            hit->hit_face = 3;
        hit->uv = (a+TILE_SIZE)/2/TILE_SIZE;
    }
    // clamp uv to 0 to 1, cause it can sometimes be slightly larger or smaller because of min hit dist.
    hit->uv = hit->uv<0?0:hit->uv;
    hit->uv = hit->uv>1?1:hit->uv;
}

#define HIT_DIST 0.005f
#define MAX_STEPS 100

int testRay(Ray *ray, Hit *hit) {
	LALGBR_Vec2d dir;
	hit->dist = 0;
	
	for(int i = 0; i < MAX_STEPS; i++) {
		if(hit->dist > RENDER_DIST) 
			break;
		hit->point.x = ray->origin.x;
		hit->point.y = ray->origin.y;
		
		dir.x = ray->direction.x;
		dir.y = ray->direction.y;
		LALGBR_MulF(&dir, hit->dist);
		LALGBR_Add(&hit->point, &dir);

		float d = getDist(&hit->point, hit);

		if(d < HIT_DIST) {
			calculateHitUVAndFace(hit);
            return 1;
        }

		hit->dist += d;
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
    Hit hit;

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
        

		if(testRay(&ray, &hit)) {
			pixel_column_rect.h = context->window_height/hit.dist;

			pixel_column_rect.y = context->window_height / 2 - pixel_column_rect.h / 2;



            int textureColumn = (hit.textureId-1)%(textures->tiles_x);
            int textureRow = ((hit.textureId-1)-textureColumn)/textures->tiles_y;
			
            //textures->sampleRect.w = 1;
            //textures->sampleRect.h = textures->tile_height;

            textures->sampleRect.x = round(hit.uv*(textures->tile_width-1));
            textures->sampleRect.x += textures->tile_width*textureColumn;
            textures->sampleRect.y = floor(textureRow*32);


            switch (hit.hit_face)
            {
            case 0:
				SDL_SetTextureColorMod(textures->image, 0xff, 0xb0, 0xb0);
                break;
            case 1:
                SDL_SetTextureColorMod(textures->image, 0xb0, 0xff, 0xb0);
                break;
            case 2: 
                SDL_SetTextureColorMod(textures->image, 0xb0, 0xb0, 0xff);
                break;
            case 3:
                SDL_SetTextureColorMod(textures->image, 0xff, 0xff, 0xff);
                break;
            default:
                break;
            }
            
			SDL_RenderCopy(context->renderer, textures->image, &textures->sampleRect, &pixel_column_rect);
			

			// SDL_SetRenderDrawColor(SDLM_renderer, ray.uv*255*(ray.textureId==2?0:1), ray.uv*255*(ray.textureId==1?0:1), 0, 0x0f);
			
		}
	}
}