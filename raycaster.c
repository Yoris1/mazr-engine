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

float getDist(LALGBR_Vec2d* p, Hit* hit, Map* map, int row) {
	float minDist = RENDER_DIST;
    float a, b, d;
	Tile t;
    for(int i = 0; i < map->tile_count; i++) {
		t = map->tiles[i];
		if(t.height <= row)
			continue;

		a = fabsf(p->x - t.x);
		b = fabsf(p->y - t.y);

		if(a > b)
			d = a;
		else
			d = b;

		if(minDist > d) {
			minDist = d;
			hit->tile_pos.x = t.x;
			hit->tile_pos.y = t.y;
			hit->textureId = t.tex_id;
		}
	};
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

    if(hit->hit_face == 1 || hit->hit_face == 2) 
            hit->uv = 1-hit->uv;
    // fix uv direction for faces that have it reversed
    
}

#define HIT_DIST 0.0075f
#define MAX_STEPS 200

int testRay(Ray *ray, Hit *hit, Map* map, int row) {
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

		float d = getDist(&hit->point, hit, map, row);

		if(d < HIT_DIST) {
			calculateHitUVAndFace(hit);
            return 1;
        }

		hit->dist += d;
	}   
	return 0;
}

SDL_Rect pixel_column_rect;

void raycast(RenderContext* context, Camera* cam, TextureAtlas* textures, int row, Map* map) {
    SDL_SetRenderTarget(context->renderer, context->target);
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
        double rayAngle = fabsf(atan(ray.direction.y/ray.direction.x));
		LALGBR_MulMat2x2(&ray.direction, &cam->rotationMatrix);

		ray.origin = cam->pos;
        
		if(testRay(&ray, &hit, map, row)) {
            float distToCameraPlane = sin(rayAngle)*hit.dist;
			pixel_column_rect.h = round(context->window_height/distToCameraPlane);
           
            pixel_column_rect.y = context->window_height / 2 - pixel_column_rect.h / 2;
            
            pixel_column_rect.y -= pixel_column_rect.h*row;

            int textureColumn = (hit.textureId-1)%(textures->tiles_x);
            int textureRow = hit.hit_face;
			
            textures->sampleRect.w = 1;
            textures->sampleRect.h = textures->tile_height;

            textures->sampleRect.x = round(hit.uv*(textures->tile_width-1));
            textures->sampleRect.x += textures->tile_width*textureColumn;
            textures->sampleRect.y = floor(textureRow*textures->tile_height);

            float darkness = hit.dist*hit.dist;
            darkness = darkness<2?2:darkness;
            float brightness = 2/darkness;
            SDL_SetTextureColorMod(textures->image, brightness*0xff, brightness*0xff, brightness*0xff);

			SDL_RenderCopy(context->renderer, textures->image, &textures->sampleRect, &pixel_column_rect);
		}
	}
}

void drawBackground(RenderContext* context) {
    SDL_SetRenderTarget(context->renderer, context->target);
    SDL_SetRenderDrawColor(context->renderer, 0x0f, 0x0f, 0xff, 0xff);
	SDL_RenderClear(context->renderer);
	SDL_SetRenderDrawColor(context->renderer, 0x0f, 0x0f, 0x0f, 0xff);
	
    pixel_column_rect.w = context->window_width;
	pixel_column_rect.x = 0;
	pixel_column_rect.y = context->window_height/2;
	pixel_column_rect.h = context->window_height;
	SDL_RenderFillRect(context->renderer, &pixel_column_rect);
}