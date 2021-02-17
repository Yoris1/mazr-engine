#include <math.h>
#include "vectors.h"
#include "raycaster.h"
#include "sdlm.h"

#define MOVE_SPEED 3
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#define TEXTURE_WIDTH 32
#define TEXTURE_HEIGHT 32

SDL_Rect texture_sample_rect;
LALGBR_Vec2d pos;

SDL_Surface* walls_surface;
SDL_Texture* walls_texture;
const Uint8* keyboardState;
void loop(float dTime, float time);
void render(SDL_Texture *t, SDL_Renderer *r);

Camera* cam;

int main(int argc, char *argv[])
{
	keyboardState = SDL_GetKeyboardState(NULL);

	pos.x = 5;
	pos.y = 5;
	cam = createCamera(pos, 107);
	if(SDLM_SetupWindowWithRenderContext("Mazr", WINDOW_WIDTH, WINDOW_HEIGHT))
		return 0;	
	walls_surface = SDL_LoadBMP("textures/walls.bmp");
	walls_texture = SDL_CreateTextureFromSurface(_SDLM_renderer, walls_surface);
	
	texture_sample_rect.w = 1;
	texture_sample_rect.h = TEXTURE_HEIGHT;
	texture_sample_rect.y = 0;

	SDLM_initGameLoop(&loop, &render);
	
	SDLM_destroy();
	free(cam);
	return 0;
}

float rot = 0;
float fov = 107;

void loop(float dTime, float time) {
	if(keyboardState[SDL_SCANCODE_A])
		rotateCamera(180*dTime, cam);
	else if(keyboardState[SDL_SCANCODE_D])
		rotateCamera(-180*dTime, cam);

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
	LALGBR_MulMat2x2(&movDir, getCameraRotationMatrix(cam));
	pos.x += movDir.x;
	pos.y += movDir.y;

	if(keyboardState[SDL_SCANCODE_S]) {
		fov += 50*dTime;
		setCameraFov(fov, cam);
	}
	else if(keyboardState[SDL_SCANCODE_W]) {
		fov -= 50*dTime;
		setCameraFov(fov, cam);
	}
	printf("FPS: %f delta time: %f \n", 1/dTime, dTime);
}

void render(SDL_Texture *t, SDL_Renderer *r){
	setCameraPosition(pos, cam);
	RenderContext cont;
	cont.renderer = r;
	cont.target = t;
	cont.window_height = WINDOW_HEIGHT;
	cont.window_width = WINDOW_WIDTH;
	raycast(&cont, cam, &texture_sample_rect, walls_texture);
}