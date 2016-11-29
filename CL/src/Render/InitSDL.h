
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <string>

bool initSDL();
SDL_Surface* loadSurfaceSDL(std::string path);
void closeSDL();