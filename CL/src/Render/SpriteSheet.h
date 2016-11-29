#pragma once

#include <SDL.h>
#include "TextureSDL.h"
#include "../Vectors.h"


class SpriteSheet {
private:
	TextureSDL texture;
	Vec2i size;
	Vec2i sprite_size;

public:
	SpriteSheet(std::string textureFilename, Vec2i size);
	~SpriteSheet();

	void renderSprite(SDL_Rect loc, int sprite_id);
	void renderSprite(SDL_Rect loc, Vec2i sprite_coord);
	void renderSprite(SDL_Rect loc, Vec2i sprite_coord, Color::Rgb color_mod);
};

SpriteSheet loadSprite(std::string filename, Vec2i sprite_dimensions);
