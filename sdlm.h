#include <SDL2/SDL.h>
// sdl manager lib
SDL_Renderer *SDLM_renderer;
SDL_Texture *SDLM_texture;
SDL_Event SDLM_event;
SDL_Window *SDLM_window;


int SDLM_SetupWindowWithRenderContext(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, 
        "Couldn't initialize SDL: %s", SDL_GetError());
		return 3;
	}
    SDLM_window = SDL_CreateWindow(title, 
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    width, height,
    SDL_WINDOW_BORDERLESS);
    
    if(SDLM_window == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
         "Couldn't create a window: %s", SDL_GetError());
        return 3;
    }

    SDLM_renderer = SDL_CreateRenderer(SDLM_window, -1, 0);
    if(SDLM_renderer == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, 
        "Couldn't create a render context: %s", SDL_GetError());
        return 3;
    }

    SDLM_texture = SDL_CreateTexture(SDLM_renderer, 
    SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 
    width, height);

    return 0;
}

int SDLM_initGameLoop(void (*loop)(float dTime, float time, SDL_Event* event)) {
    float timerResolution = SDL_GetPerformanceFrequency();
	float lastFrameTime = SDL_GetPerformanceCounter();
	float frameTime = 0;
	float deltaTime = 0;
    while(1) {
        SDL_PollEvent(&SDLM_event);
        if(SDLM_event.type == SDL_QUIT)
            return 0; // exiting on user request

        loop(deltaTime, lastFrameTime/timerResolution, &SDLM_event);

        SDL_SetRenderTarget(SDLM_renderer, NULL);
		SDL_RenderCopy(SDLM_renderer, SDLM_texture, NULL, NULL);
		SDL_RenderPresent(SDLM_renderer);

        float time = SDL_GetPerformanceCounter();
		frameTime = time - lastFrameTime;
		lastFrameTime = time;
		deltaTime = frameTime / timerResolution;
    }
}

int SDLM_destroy() {
    SDL_DestroyRenderer(SDLM_renderer);
	SDL_DestroyTexture(SDLM_texture);
	SDL_DestroyWindow(SDLM_window);
	SDL_Quit();
}