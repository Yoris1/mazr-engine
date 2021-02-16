#include <math.h>
#include "vectors.h"
#include "raycaster.h"
#include "sdlm.h"


#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#define TEXTURE_WIDTH 32
#define TEXTURE_HEIGHT 32

SDL_Rect pixel_row_rect;
SDL_Rect texture_sample_rect;
Ray ray;
vec2d pos;

SDL_Surface* walls_surface;
SDL_Texture* walls_texture;
const Uint8* keyboardState;
int main(int argc, char *argv[])
{
	keyboardState = SDL_GetKeyboardState(NULL);

	pos.x = 5;
	pos.y = 5;
	if(SDLM_SetupWindowWithRenderContext("Mazr", WINDOW_WIDTH, WINDOW_HEIGHT))
		return 0;	
	walls_surface = SDL_LoadBMP("textures/walls.bmp");
	walls_texture = SDL_CreateTextureFromSurface(SDLM_renderer, walls_surface);
	
	texture_sample_rect.w = 1;
	texture_sample_rect.h = TEXTURE_HEIGHT;
	texture_sample_rect.y = 0;

	SDLM_initGameLoop(&loop, &render);
	SDLM_destroy();
	return 0;
}
float rot = 0;
mat2x2 rotationMatrix;
void loop(float dTime, float time) {
	if(keyboardState[SDL_SCANCODE_UP])
		pos.y += 10*dTime;
	else if(keyboardState[SDL_SCANCODE_DOWN])
		pos.y -= 10*dTime;
	if(keyboardState[SDL_SCANCODE_RIGHT])
		pos.x += 10*dTime;
	else if(keyboardState[SDL_SCANCODE_LEFT])
		pos.x -= 10*dTime;
	if(keyboardState[SDL_SCANCODE_A])
		rot += 40*dTime;
	else if(keyboardState[SDL_SCANCODE_D])
		rot -= 40*dTime;
	rotationMatrix = getRotationMatrix(rot);
}
void render(SDL_Texture *texture, SDL_Renderer *renderer) {
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
		
		float aspectRatioX = WINDOW_WIDTH/WINDOW_HEIGHT;
		d.x = (float)row/WINDOW_WIDTH;
		d.y = 2;

		sub(&d, &xOffset); // -0.5 to 0.5 to camera
		d.x *= aspectRatioX;
		normalize(&d);
		mulMat2x2(&d, &rotationMatrix);

		ray.origin = pos;
		ray.direction = d;
		ray.dist = 0;
		ray.hitAxis = 0;
		if(castRay(&ray)) {
			float brightness = ray.dist*0xff/RENDER_DIST;
			brightness = 1 - brightness;
			SDL_SetRenderDrawColor(SDLM_renderer, ray.hitTextureX*255*(ray.textureId==2?0:1), ray.hitTextureX*255*(ray.textureId==1?0:1), 0, 0xff);
			pixel_row_rect.h = pow(1-(ray.dist/RENDER_DIST), 2)*WINDOW_HEIGHT; 
			pixel_row_rect.y = WINDOW_HEIGHT / 2 - pixel_row_rect.h / 2;
			texture_sample_rect.x = floor(ray.hitTextureX*TEXTURE_WIDTH+TEXTURE_WIDTH*(ray.textureId-1));
			SDL_RenderCopy(renderer, walls_texture, &texture_sample_rect, &pixel_row_rect);
			//SDL_RenderDrawRect(SDLM_renderer, &pixel_row_rect);
		}
	}
}