
#include "InitSDL.h"
#include "../Globals.h"

// Screen dimension constants
const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 1000;

bool initSDL() {
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	// Create window
	g.window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (g.window == nullptr) {
		fprintf(stderr, "Window could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}
	// Create renderer for window
	g.renderer = SDL_CreateRenderer(g.window, -1, SDL_RENDERER_ACCELERATED);
	if (g.renderer == nullptr) {
		fprintf(stderr,
			"Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	// Initialize renderer color
	SDL_SetRenderDrawColor(g.renderer, 0xFF, 0xFF, 0xFF, 0xFF);

	// Initialize PNG loading
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		fprintf(stderr,
			"SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		return false;
	}

	if (TTF_Init() == -1) {
		fprintf(stderr,
			"SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
		return false;
	}

	return true;
}

SDL_Surface* loadSurfaceSDL(std::string path) {
	// Load image at specified path
	SDL_Surface* loaded_surface = SDL_LoadBMP(path.c_str());
	if (loaded_surface == nullptr) {
		fprintf(stderr, "Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		return nullptr;
	}

	SDL_Surface* optimized_surface = nullptr;
	// Convert surface to screen format
	optimized_surface = SDL_ConvertSurface(loaded_surface, g.screen_surface->format, 0);
	if (optimized_surface == nullptr) {
		fprintf(stderr, "Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		return nullptr;
	}
	SDL_FreeSurface(loaded_surface);

	return optimized_surface;
}

void loadSpriteSheets(){
}

void closeSDL() {
	// Destroy window
	SDL_DestroyRenderer(g.renderer);
	SDL_DestroyWindow(g.window);
	g.renderer = nullptr;
	g.renderer = nullptr;

	// Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}