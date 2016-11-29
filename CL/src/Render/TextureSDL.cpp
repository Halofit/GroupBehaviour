
#include "TextureSDL.h"

#include "../Error.h"
#include <SDL.h>
#include <iostream>

TextureSDL::TextureSDL() {
	// Initialize
	tex = nullptr;
	width = 0;
	height = 0;
}

TextureSDL::~TextureSDL() {
	// Deallocate
	free();
}

// Loads image at specified path
bool TextureSDL::loadFromFile(std::string path, SDL_Renderer* renderer) {
	// free any previous held texture
	free();

	// The final texture
	SDL_Texture* new_texture = nullptr;

	// Load image at specified path
	SDL_Surface* loaded_surface = IMG_Load(path.c_str());
	if (loaded_surface == nullptr) {
		printf("Unable to load image %s! SDL_image Error: %s\n",
		       path.c_str(), IMG_GetError());
		signalError("Error loading texture.");
		return false;
	}

	// Color key image ->
	// FIXME set the color_key color to not be hardcoded -> now it's pink
	SDL_SetColorKey(loaded_surface, SDL_TRUE,
			SDL_MapRGB(loaded_surface->format, 0xFF, 0x0, 0xFF));
	// Create texture from surface pixels
	new_texture = SDL_CreateTextureFromSurface(renderer, loaded_surface);
	if (new_texture == nullptr) {
		printf("Unable to create texture from %s! SDL Error: %s\n",
		       path.c_str(), SDL_GetError());
		signalError("Error creating texture.");
		return false;
	}
	// Get image dimensions
	width = loaded_surface->w;
	height = loaded_surface->h;

	// Get rid of old loaded surface
	SDL_FreeSurface(loaded_surface);

	// Return success
	tex = new_texture;
	filename = path;

	return tex != nullptr;
}

// Deallocates texture
void TextureSDL::free() {
	// Free texture if it exists
	if (tex != nullptr) {
		std::cout << "Freeing texture " << filename << ".\n";
		SDL_DestroyTexture(tex);
		tex = nullptr;
		width = 0;
		height = 0;
	}
}

// Renders texture at given point
void TextureSDL::render(SDL_Renderer* renderer, int x, int y) const {
	// Set rendering space and render to screen
	SDL_Rect renderQuad = {x, y, width, height};
	SDL_RenderCopy(renderer, tex, nullptr, &renderQuad);
}

// Renders texture at given point
void TextureSDL::render(SDL_Renderer* renderer, SDL_Rect renderQuad) const {
	SDL_RenderCopy(renderer, tex, nullptr, &renderQuad);
}

void TextureSDL::render(SDL_Renderer* renderer, SDL_Rect location, SDL_Rect clip) const {
	SDL_RenderCopy(renderer, tex, &clip, &location);
}

void TextureSDL::render(SDL_Renderer* renderer, SDL_Rect location,
			SDL_Rect clip, Color::Rgb color_mod) const {
	SDL_SetTextureColorMod(tex, color_mod.r, color_mod.g, color_mod.b);
	SDL_RenderCopy(renderer, tex, &clip, &location);
	SDL_SetTextureColorMod(tex, 255, 255, 255);
}
