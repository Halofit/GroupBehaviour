#pragma once

#include <string>

#include "SDL.h"
#include "SDL_image.h"
#include "../Color.h"

class TextureSDL {
public:
	int width;
	int height;

private:
	std::string filename;
	SDL_Texture* tex;

public:
	TextureSDL();
	~TextureSDL();


	bool loadFromFile(std::string path, SDL_Renderer* renderer);
	void free();
	void render(SDL_Renderer* renderer, int x, int y) const;
	void render(SDL_Renderer* renderer, SDL_Rect renderQuad) const;
	void render(SDL_Renderer * renderer, SDL_Rect location, SDL_Rect clip) const;
	void render(SDL_Renderer * renderer, SDL_Rect location, SDL_Rect clip, Color::Rgb color_mod) const;
};