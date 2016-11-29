#pragma once

#include <SDL.h>
#include <SDL_ttf.h>

#include "Render/SpriteSheet.h"
#include "Simulation.h"
#include <memory>

struct Globals {
	bool quit;

	//The window we'll be rendering to
	SDL_Window* window;
	//The surface contained by the window
	SDL_Surface* screen_surface;
	//The image we will load and show on the screen
	//SDL_Surface* x_out;
	//The window renderer 
	SDL_Renderer* renderer;


	//Font for rendering text
	TTF_Font* sans_font;

	std::unique_ptr<SpriteSheet> circle_sprite;

	Simulation simulation;
};

extern Globals g;
const double pi = 3.14159265358979323846;
const float pi_f = static_cast<float>(pi);


//(SDL) functions in Main.cpp that should be visible from other places
Vec2i getWindowSize();
