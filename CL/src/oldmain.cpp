
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <algorithm>
#include <ctime>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <stdio.h>
#include <vector>

#include "Color.h"
#include "Error.h"
#include "Globals.h"
#include "Render/InitSDL.h"
#include "Render/Sprite.h"
#include "Render/TextureSDL.h"
#include "Render/RenderText.h"
#include "Utils.h"
#include "UI/Button.h"

Globals g;

Vec2i getWindowSize() {
	int w, h;
	SDL_GetWindowSize(g.window, &w, &h);
	return Vec2i(w, h);
}

Vec2i getMousePosition() {
	int x, y;
	SDL_GetMouseState(&x, &y);
	return Vec2i(x, y);
}

// Only scale
Vec2f scaleGameToScreen(Vec2f loc) {
	Game::Instance& gi = g.current_instance;
	Vec2f ret = {loc.x * gi.pixels_per_metre, loc.y * gi.pixels_per_metre};
	return ret;
}

// Scale back
Vec2f scaleScreenToGame(Vec2f loc) {
	Game::Instance& gi = g.current_instance;
	Vec2f ret = {loc.x / gi.pixels_per_metre, loc.y / gi.pixels_per_metre};
	return ret;
}

// Move and scale
Vec2f gameToScreen(Vec2f loc) {
	Game::Instance& gi = g.current_instance;

	// Scale first
	Vec2f ret = scaleGameToScreen(loc);

	// Normalise and invert y coordinate
	ret = ret - gi.camera_center;
	ret.y = -ret.y;

	// Now shift -> this centers the view,
	// but the SDL screen starts in top-left
	auto ws = getWindowSize();
	Vec2f wsf;
	wsf.x = ws.x / 2;
	wsf.y = ws.y / 2;

	ret = ret + wsf;

	return ret;
}

Vec2f screenToGame(Vec2f loc) {
	Game::Instance& gi = g.current_instance;
	Vec2f ret;

	// shift
	auto ws = getWindowSize();
	Vec2f wsf;
	wsf.x = ws.x / 2;
	wsf.y = ws.y / 2;
	ret = loc - wsf;

	// De-normalise and invert y coordinate
	ret = ret + gi.camera_center;
	ret.y = -ret.y;

	// Finally scale
	ret = scaleScreenToGame(ret);

	return ret;
}

void handleKeyboardInput(SDL_Event e) {
	switch (e.key.keysym.sym) {
		case SDLK_UP:
			g.current_instance.move_player(Vec2f{0.f, 1.f});
			break;
		case SDLK_DOWN:
			g.current_instance.move_player(Vec2f{0.f, -1.f});
			break;
		case SDLK_LEFT:
			g.current_instance.move_player(Vec2f{-1.f, 0.f});
			break;
		case SDLK_RIGHT:
			g.current_instance.move_player(Vec2f{1.f, 0.f});
			break;

		case SDLK_v: {
			auto mp = getMousePosition();
			Vec2f pos;
			pos.x = mp.x;
			pos.y = mp.y;

			pos = screenToGame(pos);
			logVar(mp.toString());
			logVar(pos.toString());

			g.current_instance.map.addNpc(pos);
		} break;

		case SDLK_ESCAPE:
			g.quit = true;
			break;
	}
}

std::vector<UI::Button> ui_buttons;

void handleMouseClick(SDL_Event e) {
	Vec2i loc{e.button.x, e.button.y};
	switch (e.button.button) {
		case SDL_BUTTON_LEFT: {
			auto button = std::find_if(
			    ui_buttons.rbegin(), ui_buttons.rend(),
			    [&loc](const UI::Button& b) -> bool {
				    return (b.x1 < loc.x && b.y1 < loc.y &&
					    b.x2 > loc.x && b.y2 > loc.y);
			    });

			if (e.type == SDL_MOUSEBUTTONDOWN) {
				// If there is a button there
				if (button != ui_buttons.rend()) {
					button->press();
				}
			} else if (e.type == SDL_MOUSEBUTTONUP) {
				if (button != ui_buttons.rend()) {
					button->depress();
				} else {
					for (auto& b : ui_buttons)
						b.reset();
				}
			}
		} break;
		case SDL_BUTTON_RIGHT:
			logVar(loc);
			Vec2f locf{loc.x, loc.y};
			logVar(screenToGame(locf));
			break;
	}
}

void renderObjects() {
	auto drawables = g.current_instance.getDrawables();
	// logVar(drawables.size());

	for (auto& d : drawables) {
		d.loc = gameToScreen(d.loc);
		d.size = scaleGameToScreen(d.size);

		// logVar(d.loc.toString());
	}

	for (const auto& d : drawables) {
		Vec2f tlc = d.loc - (d.size * 0.5f);
		SDL_Rect loc = {tlc.x, tlc.y, d.size.x, d.size.y};

		auto& sprite_sheet = g.ss_dwarf_fort;
		sprite_sheet->renderSprite(loc, d.sprite_id);
	}
}

void renderUIElements() {
	for (const auto& b : ui_buttons) {
		SDL_Rect loc = {b.x1, b.y1, b.x2 - b.x1, b.y2 - b.y1};

		int modifier = b.position + 1;
		Color::Rgb color_mod{100 * modifier, 100 * modifier, 100 * modifier};

		auto& sprite_sheet = g.ss_dwarf_fort;
		sprite_sheet->renderSprite(loc, Vec2i{11, 13}, color_mod);

		Vec2i letter_size{(b.y2 - b.y1 - 3) / 2, b.y2 - b.y1 - 3};
		Vec2i text_centre{b.x1 + (b.x2 - b.x1) / 2, b.y1 + (b.y2 - b.y1) / 2};

		renderText(b.button_text, letter_size, text_centre);
	}
}

int main(int argc, char* args[]) {
	// Start up SDL and create window
	if (!initSDL()) {
		printf("Failed to initialize!\n");
		return -1;
	}

	// Wrap everything but init and cleanup in a scope
	// just to trigger any local destructors before the call to cleanup
	{

		g.ss_dwarf_fort = std::make_unique<SpriteSheet>(
		    "resources/dfSheet32.png", Vec2i{16, 16});

		// this opens a font style and sets a size
		g.sans_font = TTF_OpenFont("C:/Windows/Fonts/micross.ttf", 50);
		if (g.sans_font == nullptr) signalError("Font not found.");

		// Setup game world
		// g.world.setup();

		// Game loop setup: timing, event handlers ...
		double tick_time = 0.016f;
		std::clock_t start_time;
		std::clock_t curr_time;
		double duration;
		start_time = std::clock();
		size_t tick_counter = 0;

		// Event handler
		SDL_Event sdl_event;

		// Add some ui
		ui_buttons.push_back(UI::Button(
		    0, 0, 300,
		    20, [](int i) { std::cout << "Hello there " << i << "\n"; }, "Hello"));

		// While application is running
		g.quit = false;
		while (!g.quit) {
			tick_counter++;

			// Handle events on queue
			while (SDL_PollEvent(&sdl_event) != 0) {
				// Check events
				switch (sdl_event.type) {
					case SDL_QUIT:
						g.quit = true;
						break;
					case SDL_KEYDOWN:
						handleKeyboardInput(sdl_event);
						break;
					case SDL_MOUSEBUTTONDOWN:
					case SDL_MOUSEBUTTONUP:
						handleMouseClick(sdl_event);
						break;
				}
			}

			// g.world.tick();

			// Clear screen
			SDL_SetRenderDrawColor(g.renderer, 0, 0, 0, 0);
			SDL_RenderClear(g.renderer);

			renderObjects();

			// Draw some UI
			std::stringstream ss;
			ss << tick_counter;
			renderText(leftPad(ss.str(), 4), {10, 18}, PositionAlignment::BOT_LEFT);

			ss.str(std::string()); // clear ss
			// ss << g.world.ship.loc.x << ", " <<
			// g.world.ship.loc.y;
			renderText(leftPad(ss.str(), 4), {10, 18},
				   PositionAlignment::BOT_RIGHT);

			renderUIElements();

			// Update screen
			SDL_RenderPresent(g.renderer);

			// Check timer
			curr_time = std::clock();
			duration = (curr_time - start_time) / (double)CLOCKS_PER_SEC;

			if (duration < tick_time) {
				Uint32 delay = (Uint32)((tick_time - duration) * 1000.0);
				SDL_Delay(delay);
			} else {
				ss.str(std::string()); // clear ss
				ss << "Going too slow by "
				   << (duration - tick_time) * 1000 << "ms";
				logError(ss.str());
			}

			start_time = std::clock();
		}
	}

	// Free resources and close SDL -- NO!
	closeSDL();

	return 0;
}
