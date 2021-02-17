#include <math.h>
#include "vectors.h"
#include "raycaster.h"
#include "sdlm.h"

#define MOVE_SPEED 3
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#define TEXTURE_WIDTH 32
#define TEXTURE_HEIGHT 32

SDL_Rect pixel_row_rect;
SDL_Rect texture_sample_rect;
Ray ray;
LALGBR_Vec2d pos;

float fov = 0;
float fov_deg = 107;
// definitely should move to it's own c file, this has no place in main.
void updateFOV() {
	fov = atan(M_PI/180*(fov_deg/2))*2;
}

SDL_Surface* walls_surface;
SDL_Texture* walls_texture;
const Uint8* keyboardState;
void loop(float dTime, float time);
void render(SDL_Texture *texture, SDL_Renderer *renderer);

int main(int argc, char *argv[])
{
	updateFOV();
	keyboardState = SDL_GetKeyboardState(NULL);

	pos.x = 5;
	pos.y = 5;
	if(SDLM_SetupWindowWithRenderContext("Mazr", WINDOW_WIDTH, WINDOW_HEIGHT))
		return 0;	
	walls_surface = SDL_LoadBMP("textures/walls.bmp");
	walls_texture = SDL_CreateTextureFromSurface(_SDLM_renderer, walls_surface);
	
	texture_sample_rect.w = 1;
	texture_sample_rect.h = TEXTURE_HEIGHT;
	texture_sample_rect.y = 0;

	SDLM_initGameLoop(&loop, &render);
	SDLM_destroy();
	return 0;
}

float rot = 0;
LALGBR_Mat2x2 rotationMatrix;
void loop(float dTime, float time) {
	if(keyboardState[SDL_SCANCODE_A])
		rot += 180*dTime;
	else if(keyboardState[SDL_SCANCODE_D])
		rot -= 180*dTime;
	rotationMatrix = LALGBR_GetRotationMatrix(rot);

	LALGBR_Vec2d movDir;
	movDir.x = 0;
	movDir.y = 0;
	
	if(keyboardState[SDL_SCANCODE_UP])
		movDir.y =+ MOVE_SPEED;
	else if(keyboardState[SDL_SCANCODE_DOWN])
		movDir.y =- MOVE_SPEED;
	if(keyboardState[SDL_SCANCODE_RIGHT])
		movDir.x =+ MOVE_SPEED;
	else if(keyboardState[SDL_SCANCODE_LEFT])
		movDir.x =- MOVE_SPEED;
	
	LALGBR_MulF(&movDir, dTime);	
	LALGBR_MulMat2x2(&movDir, &rotationMatrix);
	pos.x += movDir.x;
	pos.y += movDir.y;

	if(keyboardState[SDL_SCANCODE_S]) {
		fov_deg += 50*dTime;
		updateFOV();
	}
	else if(keyboardState[SDL_SCANCODE_W]) {
		fov_deg -= 50*dTime;
		updateFOV();
	}
}

void render(SDL_Texture *texture, SDL_Renderer *renderer) {
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_MUL);
	
	SDL_SetRenderTarget(renderer, _SDLM_texture);

	SDL_SetRenderDrawColor(renderer, 0x0f, 0x0f, 0xff, 0xff);
	SDL_RenderClear(renderer);
	
	SDL_SetRenderDrawColor(renderer, 0x0f, 0x0f, 0x0f, 0xff);
	pixel_row_rect.w = WINDOW_WIDTH;
	pixel_row_rect.x = 0;

	pixel_row_rect.y = WINDOW_HEIGHT/2;
	pixel_row_rect.h = WINDOW_HEIGHT;
	SDL_RenderFillRect(renderer, &pixel_row_rect);

	
	LALGBR_Vec2d d;
	pixel_row_rect.w = 1;
	
	for(int row = 0; row < WINDOW_WIDTH; row++) {
		pixel_row_rect.x = row;
		
		d.x = (float)row/WINDOW_WIDTH;
		d.x -= 0.5f;
		d.y = 1.0f;
		d.x *= WINDOW_WIDTH/WINDOW_HEIGHT;
		
		d.x *= fov;

		LALGBR_Normalize(&d);
		LALGBR_MulMat2x2(&d, &rotationMatrix);

		ray.origin = pos;
		ray.direction = d;
		ray.dist = 0;
		ray.hitAxis = 0;
		if(castRay(&ray)) {
			pixel_row_rect.h = WINDOW_HEIGHT/ray.dist;

			pixel_row_rect.y = WINDOW_HEIGHT / 2 - pixel_row_rect.h / 2;
			texture_sample_rect.x = floor(ray.hitTextureX*TEXTURE_WIDTH+TEXTURE_WIDTH*(ray.textureId-1));
			SDL_RenderCopy(renderer, walls_texture, &texture_sample_rect, &pixel_row_rect);
			

			// SDL_SetRenderDrawColor(SDLM_renderer, ray.hitTextureX*255*(ray.textureId==2?0:1), ray.hitTextureX*255*(ray.textureId==1?0:1), 0, 0x0f);
			if(ray.hitAxis){
				SDL_SetRenderDrawColor(renderer, 70, 70, 70, 0x70);
				SDL_RenderDrawRect(renderer, &pixel_row_rect);
			}
		}
	}
}