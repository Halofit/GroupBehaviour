#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <stdio.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <memory>
#include <algorithm>

#include "Globals.h"
#include "Error.h"
#include "Utils.h"
#include "Render/InitSDL.h"
#include "Render/RenderText.h"
#include "Render/SpriteSheet.h"


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


// Screen dimension constants
const float world_multiplier = 20.f;
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;


void addAgentsResp(int num) {
	auto p = getMousePosition();
	Vec2f loc{p.x / world_multiplier, p.y / world_multiplier};
	g.simulation.addAgents(loc, num);
}

void handleKeyboardInput(SDL_Event e) {
	switch (e.key.keysym.sym) {
		case SDLK_ESCAPE:
			g.quit = true;
			break;

		case SDLK_c: {
			static Vec2f start;
			static bool drawn = true;

			auto p = getMousePosition();
			Vec2f loc{p.x / world_multiplier, p.y / world_multiplier};

			if (drawn) {
				start = loc;
				drawn = false;
			} else {
				g.simulation.addWall(start, loc);
				drawn = true;
			}
			break;
		}

		case SDLK_1: {
			addAgentsResp(1);
			break;
		}

		case SDLK_2: {
			addAgentsResp(2);
			break;
		}

		case SDLK_3: {
			addAgentsResp(3);
			break;
		}

		case SDLK_4: {
			addAgentsResp(4);
			break;
		}

		case SDLK_5: {
			addAgentsResp(5);
			break;
		}

		case SDLK_6: {
			addAgentsResp(6);
			break;
		}

		case SDLK_7: {
			addAgentsResp(7);
			break;
		}

		case SDLK_8: {
			addAgentsResp(8);
			break;
		}

		case SDLK_9: {
			addAgentsResp(9);
			break;
		}

		case SDLK_0: {
			addAgentsResp(10);
			break;
		}



		case SDLK_g: {
			g.simulation.clear();
			break;
		}
		case SDLK_f: {
			g.simulation.clearAgents();
			break;
		}
		case SDLK_y: {
			auto p = getMousePosition();
			Vec2f loc{p.x / world_multiplier, p.y / world_multiplier};

			g.simulation.target_location = loc;
			break;
		}
	}
}


void renderObjects() {
	auto agents = g.simulation.getAgents();
	auto walls = g.simulation.getWalls();

	for (auto& ob : *agents) {
		Vec2f l = ob.location - Vec2f{ob.diameter / 2, ob.diameter / 2};
		l = l * world_multiplier;
		int size = static_cast<int>(ob.diameter * world_multiplier);
		Vec2i li{static_cast<int>(l.x), static_cast<int>(l.y)};

		g.circle_sprite->renderSprite({li.x, li.y, size, size}, {0,0}, {100,0,0});
	}

	for (auto& w : *walls) {
		Vec2f s = w.locationA * world_multiplier;
		Vec2f e = w.locationB * world_multiplier;

		SDL_SetRenderDrawColor(g.renderer, 255, 255, 255, 0);
		SDL_RenderDrawLine(g.renderer,
							static_cast<int>(s.x),
							static_cast<int>(s.y),
							static_cast<int>(e.x),
							static_cast<int>(e.y));

	}
}


int main(int argc, char* args[]) {
	//Start up SDL and create window
	if (!initSDL(SCREEN_WIDTH, SCREEN_HEIGHT)) {
		printf("Failed to initialize!\n");
		return -1;
	}

	//Wrap everything but init and cleanup in a scope
	//just to trigger any local destructors before the call to cleanup
	{
		g.circle_sprite = std::make_unique<SpriteSheet>("circle.png", Vec2i{1,1});

		// this opens a font style and sets a size
		g.sans_font = TTF_OpenFont("C:/Windows/Fonts/micross.ttf", 50);
		if (g.sans_font == nullptr) signalError("Font not found.");


		//Main loop setup: timing, event handlers ...
		double tick_time = 0.016f;
		std::clock_t start_time;
		std::clock_t curr_time;
		double duration;
		start_time = std::clock();
		size_t tick_counter = 0;

		//Event handler
		SDL_Event sdl_event;

		Vec2i ws = getWindowSize();
		Vec2f world_size{ws.x / world_multiplier, ws.y / world_multiplier};
		g.simulation.init(world_size);

		//While application is running
		g.quit = false;
		while (!g.quit) {
			tick_counter++;

			//Handle events on queue
			while (SDL_PollEvent(&sdl_event) != 0) {
				//User requests quit
				if (sdl_event.type == SDL_QUIT) {
					g.quit = true;
				} else if (sdl_event.type == SDL_KEYDOWN) {
					handleKeyboardInput(sdl_event);
				}
			}

			//Clear screen 
			SDL_SetRenderDrawColor(g.renderer, 0, 0, 0, 0);
			SDL_RenderClear(g.renderer);

			renderObjects();

			//Draw some UI
			std::stringstream ss;
			ss << g.simulation.elapsed_time << "(" << tick_counter << ")";
			renderText(ss.str(), {10,18}, PositionAlignment::BOT_LEFT);

			ss.str(std::string()); //clear ss
			ss << g.simulation.agents.size();
			renderText(ss.str(), {10,18}, PositionAlignment::BOT_RIGHT);


			//Update screen 
			SDL_RenderPresent(g.renderer);

			//Check timer
			curr_time = std::clock();
			duration = (curr_time - start_time) / static_cast<double>(CLOCKS_PER_SEC);

			if (duration < tick_time) {
				Uint32 delay = Uint32((tick_time - duration) * 1000.0);
				SDL_Delay(delay);
			} else {
				ss.str(std::string()); //clear ss
				ss << "Going too slow by " << (duration - tick_time) * 1000 << "ms";
				logError(ss.str());
			}


			g.simulation.tick(0.016f);


			start_time = std::clock();
		}
	}

	//Free resources and close SDL
	closeSDL();

	return 0;
}
