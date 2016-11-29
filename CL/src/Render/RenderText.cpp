#include "RenderText.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "../Globals.h"

void renderText(std::string text, Vec2i letter_size,
		PositionAlignment alignement) {
	if (!g.sans_font) {
		fprintf(stderr, "TTF_OpenFont: %s\n", TTF_GetError());
		return;
	}

	Vec2i off;
	Vec2i surface_size = getWindowSize();

	switch (alignement) {
	case PositionAlignment::TOP_LEFT:
		off.x = 0;
		off.y = 0;
		break;
	case PositionAlignment::TOP_RIGHT:
		off.x = (int)(surface_size.x - letter_size.x * text.length());
		off.y = 0;
		break;
	case PositionAlignment::BOT_LEFT:
		off.x = 0;
		off.y = (int)(surface_size.y - letter_size.y);
		break;
	case PositionAlignment::BOT_RIGHT:
		off.x = (int)(surface_size.x - letter_size.x * text.length());
		off.y = (int)(surface_size.y - letter_size.y);
		break;
	}

	SDL_Rect ressage_rect;  // create a rect
	ressage_rect.x = off.x; // controls the rect's x coordinate
	ressage_rect.y = off.y; // controls the rect's y coordinte
	ressage_rect.w = static_cast<int>(
	    letter_size.x * text.length()); // controls the width of the rect
	ressage_rect.h = letter_size.y;     // controls the height of the rect

	// this is the color in rgb format, maxing out all would give
	// you the color white, and it will be your text's color
	SDL_Color green = {0, 255, 0};

	// as TTF_RenderText_Solid could only be used on SDL_Surface then you
	// have
	// to create the surface first
	SDL_Surface* surface_message =
	    TTF_RenderText_Solid(g.sans_font, text.c_str(), green);
	// SDL_Surface* surface_message = TTF_RenderText_Blended(sans_font,
	// text.c_str(), green);

	// now you can convert it into a texture
	SDL_Texture* message_tex = SDL_CreateTextureFromSurface(g.renderer, surface_message);

	// Mind you that (0,0) is on the top left of the window/screen,
	// think a rect as the text's box, that way it would be very simple to
	// understance
	// Now since it's a texture, you have to put RenderCopy in your game
	// loop
	// area,
	// the area where the whole code executes

	// you put the renderer's name first, the Message, the crop size
	// (you can ignore this if you don't want to dabble with cropping),
	// and the rect which is the size and coordinate of your texture
	SDL_RenderCopy(g.renderer, message_tex, nullptr, &ressage_rect);

	SDL_FreeSurface(surface_message);
	SDL_DestroyTexture(message_tex);
}

void renderText(std::string text, Vec2i letter_size, Vec2i text_centre) {
	if (!g.sans_font) {
		fprintf(stderr, "TTF_OpenFont: %s\n", TTF_GetError());
		return;
	}

	auto text_width = letter_size.x * text.length();
	auto text_height = letter_size.y;

	SDL_Rect message_rect; // create a rect
	message_rect.x = text_centre.x -
			 (static_cast<int>(text_width) / 2); // controls the rect's x coordinate
	message_rect.y = text_centre.y -
			 (text_height / 2); // controls the rect's y coordinte
	message_rect.w = static_cast<int>(text_width);	// controls the width of the rect
	message_rect.h = static_cast<int>(text_height);       // controls the height of the rect

	// this is the color in rgb format, maxing out all would give
	// you the color white, and it will be your text's color
	SDL_Color green = {0, 255, 0};

	// as TTF_RenderText_Solid could only be used on SDL_Surface then you
	// have
	// to create the surface first
	SDL_Surface* surface_message =
	    TTF_RenderText_Solid(g.sans_font, text.c_str(), green);
	// SDL_Surface* surface_message = TTF_RenderText_Blended(sans_font,
	// text.c_str(), green);

	// now you can convert it into a texture
	SDL_Texture* message_tex = SDL_CreateTextureFromSurface(g.renderer, surface_message);

	// Mind you that (0,0) is on the top left of the window/screen,
	// think a rect as the text's box, that way it would be very simple to
	// understance
	// Now since it's a texture, you have to put RenderCopy in your game
	// loop
	// area,
	// the area where the whole code executes

	// you put the renderer's name first, the Message, the crop size
	// (you can ignore this if you don't want to dabble with cropping),
	// and the rect which is the size and coordinate of your texture
	SDL_RenderCopy(g.renderer, message_tex, nullptr, &message_rect);

	SDL_FreeSurface(surface_message);
	SDL_DestroyTexture(message_tex);
}