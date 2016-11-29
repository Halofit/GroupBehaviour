#pragma once

#include <stdint.h>

namespace Color {
struct Rgb {
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

struct Hsv {
	uint8_t h;
	uint8_t s;
	uint8_t v;
};

// TODO: credit guy on stackoverflow
inline Rgb hsvToRgb(Hsv hsv) {
	Rgb rgb;
	uint8_t region, remainder, p, q, t;

	if (hsv.s == 0) {
		rgb.r = hsv.v;
		rgb.g = hsv.v;
		rgb.b = hsv.v;
		return rgb;
	}

	region = hsv.h / 43;
	remainder = (hsv.h - (region * 43)) * 6;

	p = (hsv.v * (255 - hsv.s)) >> 8;
	q = (hsv.v * (255 - ((hsv.s * remainder) >> 8))) >> 8;
	t = (hsv.v * (255 - ((hsv.s * (255 - remainder)) >> 8))) >> 8;

	switch (region) {
		case 0:
			rgb.r = hsv.v;
			rgb.g = t;
			rgb.b = p;
			break;
		case 1:
			rgb.r = q;
			rgb.g = hsv.v;
			rgb.b = p;
			break;
		case 2:
			rgb.r = p;
			rgb.g = hsv.v;
			rgb.b = t;
			break;
		case 3:
			rgb.r = p;
			rgb.g = q;
			rgb.b = hsv.v;
			break;
		case 4:
			rgb.r = t;
			rgb.g = p;
			rgb.b = hsv.v;
			break;
		default:
			rgb.r = hsv.v;
			rgb.g = p;
			rgb.b = q;
			break;
	}

	return rgb;
}
};
