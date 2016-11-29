
#include "SpriteSheet.h"
#include "../Globals.h"
#include "../Error.h"


SpriteSheet::SpriteSheet(std::string textureFilename, Vec2i size) {
	texture.loadFromFile(textureFilename, g.renderer);

	this->size = size;

	if (texture.width % size.x != 0 || texture.height % size.y != 0) {
		logError("Texture size not divisible by the "
				 "spritesheet size.");
	}

	sprite_size = Vec2i{texture.width / size.x, texture.height / size.y};
}

SpriteSheet::~SpriteSheet() {
	texture.free();
}

void SpriteSheet::renderSprite(SDL_Rect loc, int sprite_id) {
	int row = sprite_id / size.x;
	int col = sprite_id % size.x;

	if (row > size.y) signalError("Invalid sprite Id.");

	SDL_Rect clip = {col * sprite_size.x, row * sprite_size.y,
		sprite_size.x, sprite_size.y};

	texture.render(g.renderer, loc, clip);
}

void SpriteSheet::renderSprite(SDL_Rect loc, Vec2i sprite_coord) {
	int row = sprite_coord.y;
	int col = sprite_coord.x;

	if (row > size.y) signalError("Invalid sprite Id.");

	SDL_Rect clip = {col * sprite_size.x, row * sprite_size.y,
		sprite_size.x, sprite_size.y};

	texture.render(g.renderer, loc, clip);
}

void SpriteSheet::renderSprite(SDL_Rect loc, Vec2i sprite_coord, Color::Rgb color_mod) {
	int row = sprite_coord.y;
	int col = sprite_coord.x;

	if (row > size.y) signalError("Invalid sprite Id.");

	SDL_Rect clip = {col * sprite_size.x, row * sprite_size.y,
		sprite_size.x, sprite_size.y};

	texture.render(g.renderer, loc, clip, color_mod);
}