#include <math.h>
#include "vectors.h"
#include "raycaster.h"
#include "sdlm.h"
#include "texture_atlas.h"
#include "map.h"

#define MOVE_SPEED 3
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#define TEXTURE_WIDTH 32
#define TEXTURE_HEIGHT 32

SDL_Rect texture_sample_rect;
LALGBR_Vec2d pos;

const Uint8* keyboardState;
void loop(double dTime, float time);
void render(SDL_Texture *t, SDL_Renderer *r);

Camera* cam;
TextureAtlas* textures;
Map* map;
int main(int argc, char *argv[])
{
	keyboardState = SDL_GetKeyboardState(NULL);

	pos.x = 5;
	pos.y = 5;
	cam = createCamera(pos, 107);
	
	if(SDLM_SetupWindowWithRenderContext("Mazr", WINDOW_WIDTH, WINDOW_HEIGHT))
		return 0;	
	map = loadMap("res/maps/main.bin");
	textures = loadAtlas(_SDLM_renderer, "res/textures/walls_atlas.bmp", 
		TEXTURE_WIDTH, TEXTURE_HEIGHT, 2, 1);
	SDLM_SetTargetFPS(25);
	SDLM_initGameLoop(&loop, &render);
	
	free(cam);
	destroyAtlas(textures);
	SDLM_destroy();
	return 0;
}

float rot = 0;
float fov = 107;

void loop(double dTime, float time) {
	if(keyboardState[SDL_SCANCODE_A])
		rotateCamera(180*dTime, cam);
	else if(keyboardState[SDL_SCANCODE_D])
		rotateCamera(-180*dTime, cam);

	LALGBR_Vec2d movDir;
	movDir.x = 0;
	movDir.y = 0;
	
	if(keyboardState[SDL_SCANCODE_UP])
		movDir.y = 1;
	else if(keyboardState[SDL_SCANCODE_DOWN])
		movDir.y =-1;
	if(keyboardState[SDL_SCANCODE_RIGHT])
		movDir.x = 1;
	else if(keyboardState[SDL_SCANCODE_LEFT])
		movDir.x =-1;
	
	if(movDir.x != 0 && movDir.y != 0)
		LALGBR_Normalize(&movDir);
	LALGBR_MulF(&movDir, dTime*MOVE_SPEED);	
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
	drawBackground(&cont);
	raycast(&cont, cam, textures, 1, map);
	raycast(&cont, cam, textures, 0, map);
}