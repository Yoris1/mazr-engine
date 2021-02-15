#include <SDL2/SDL.h>
#include <math.h>
#include "vectors.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define FOV 90

SDL_Renderer *renderer;
SDL_Texture *texture;
SDL_Event event;
SDL_Rect pixel_row_rect;
void loop(float dTime, float time);
int main(int argc, char *argv[])
{
	SDL_Window *window;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
		return 3;
	}

	window = SDL_CreateWindow("M-A-Z-R",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		SDL_WINDOW_BORDERLESS);
	if(window == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create a window: %s", SDL_GetError());
	}

	renderer = SDL_CreateRenderer(window, -1, 0);
	
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WINDOW_WIDTH, WINDOW_HEIGHT);

	float timerResolution = SDL_GetPerformanceFrequency();
	float lastFrameTime = SDL_GetPerformanceCounter();
	float frameTime = 0;
	float deltaTime = 0;

	pixel_row_rect.h = 300;
	pixel_row_rect.w = 1;
	pixel_row_rect.y = 100;

	while (1) {

		SDL_PollEvent(&event);
		if(event.type == SDL_QUIT)
			break;
		
		loop(deltaTime, lastFrameTime/timerResolution);

		SDL_SetRenderTarget(renderer, NULL);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);

		float time = SDL_GetPerformanceCounter();
		frameTime = time - lastFrameTime;
		lastFrameTime = time;
		deltaTime = frameTime / timerResolution;
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyTexture(texture);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}

void loop(float dTime, float time) {
	SDL_SetRenderTarget(renderer, texture);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff);
	SDL_RenderClear(renderer);
	for(int row = 0; row < WINDOW_WIDTH; row++) {
		double radians = (float)row / (float)WINDOW_WIDTH * (float)FOV* M_PI / 180.0; 
		if(1) {
			pixel_row_rect.x = row;
			/* TODO instead of using sin/cos to calculate ray direction, put a line of equally spaced dots
			on one axis and the other axis is the same as camera, then move camera back some amount on the axis that's the same.
			That should result in nicely pointing rays without cos/sin operations so it should be a little easier
			*/
			SDL_SetRenderDrawColor(renderer, cos(radians)*0xff, sin(radians)*0xff, 0xff, 0xff);
			SDL_RenderDrawRect(renderer, &pixel_row_rect);
		}
	}
}