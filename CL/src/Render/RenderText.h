#pragma once

#include "../Vectors.h"
#include <string>


enum class PositionAlignment { TOP_RIGHT, BOT_RIGHT, TOP_LEFT, BOT_LEFT };

void renderText(std::string text, Vec2i letter_size,
		PositionAlignment alignement = PositionAlignment::TOP_LEFT);
void renderText(std::string text, Vec2i letter_size, Vec2i text_centre);