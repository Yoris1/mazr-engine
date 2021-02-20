#pragma once
#include <SDL2/SDL.h>
// sdl manager lib
SDL_Renderer *_SDLM_renderer;
SDL_Texture *_SDLM_texture;
SDL_Event _SDLM_event;
SDL_Window *_SDLM_window;

int SDLM_SetupWindowWithRenderContext(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, 
        "Couldn't initialize SDL: %s", SDL_GetError());
		return 3;
	}
    _SDLM_window = SDL_CreateWindow(title, 
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    width, height, SDL_WINDOW_BORDERLESS);
    
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

int SDLM_initGameLoop(void (*loop)(double dTime, float time), 
    void (*renderLoop)(SDL_Texture *t, SDL_Renderer *r), float* targetFramerate) {
        Uint64 timerResolution = SDL_GetPerformanceFrequency();
        Uint64 lastFrameTime = SDL_GetPerformanceCounter();
        float frameTime = 0;
        double deltaTime = 0;
        while(1) {
            while(SDL_PollEvent(&_SDLM_event))
            // only poll for quit event, don't care about the rest for now
                if(_SDLM_event.type == SDL_QUIT)
                    return 0; // exiting on user request

            renderLoop(_SDLM_texture, _SDLM_renderer);
            loop(deltaTime, (float)lastFrameTime/(float)timerResolution);

            SDL_SetRenderTarget(_SDLM_renderer, NULL);
            SDL_RenderCopy(_SDLM_renderer, _SDLM_texture, NULL, NULL);
            SDL_RenderPresent(_SDLM_renderer);

            Uint64 time = SDL_GetPerformanceCounter();
            frameTime = time - lastFrameTime;
            if(frameTime<timerResolution/(*targetFramerate)) {
                SDL_Delay((timerResolution/(*targetFramerate)-frameTime)/timerResolution*1000);
                time = SDL_GetPerformanceCounter();
                frameTime = time - lastFrameTime;
            } // TODO make this framerate stuff somehow better integrated then uploda to git

            lastFrameTime = time;
            deltaTime = (double)frameTime / (double)timerResolution;
        }
}

int SDLM_destroy() {
    SDL_DestroyRenderer(_SDLM_renderer);
	SDL_DestroyTexture(_SDLM_texture);
	SDL_DestroyWindow(_SDLM_window);
	SDL_Quit();
}