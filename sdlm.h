#pragma once
#include <SDL2/SDL.h>
// sdl manager lib
SDL_Renderer *_SDLM_renderer;
SDL_Texture *_SDLM_texture;
SDL_Event _SDLM_event;
SDL_Window *_SDLM_window;

// declarations for signatures of the game and render loop to be used with sdlm
void loop(float dTime, float time); 
void render(SDL_Texture *texture, SDL_Renderer *renderer);

int SDLM_SetupWindowWithRenderContext(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, 
        "Couldn't initialize SDL: %s", SDL_GetError());
		return 3;
	}
    _SDLM_window = SDL_CreateWindow(title, 
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    width, height,
    SDL_WINDOW_BORDERLESS);
    
    if(_SDLM_window == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
         "Couldn't create a window: %s", SDL_GetError());
        return 3;
    }

    _SDLM_renderer = SDL_CreateRenderer(_SDLM_window, -1, 0);
    if(_SDLM_renderer == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, 
        "Couldn't create a render context: %s", SDL_GetError());
        return 3;
    }

    _SDLM_texture = SDL_CreateTexture(_SDLM_renderer, 
    SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 
    width, height);

    return 0;
}

int SDLM_initGameLoop(void (*loop)(float dTime, float time), 
    void (*renderLoop)(SDL_Texture *t, SDL_Renderer *r)) {
        float timerResolution = SDL_GetPerformanceFrequency();
        float lastFrameTime = SDL_GetPerformanceCounter();
        float frameTime = 0;
        float deltaTime = 0;
        while(1) {
            while(SDL_PollEvent(&_SDLM_event))
            // only poll for quit event, don't care about the rest for now
                if(_SDLM_event.type == SDL_QUIT)
                    return 0; // exiting on user request

            renderLoop(_SDLM_texture, _SDLM_renderer);
            loop(deltaTime, lastFrameTime/timerResolution);

            SDL_SetRenderTarget(_SDLM_renderer, NULL);
            SDL_RenderCopy(_SDLM_renderer, _SDLM_texture, NULL, NULL);
            SDL_RenderPresent(_SDLM_renderer);

            float time = SDL_GetPerformanceCounter();
            frameTime = time - lastFrameTime;
            lastFrameTime = time;
            deltaTime = frameTime / timerResolution;
        }
}

int SDLM_destroy() {
    SDL_DestroyRenderer(_SDLM_renderer);
	SDL_DestroyTexture(_SDLM_texture);
	SDL_DestroyWindow(_SDLM_window);
	SDL_Quit();
}