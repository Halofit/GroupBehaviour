#include "Random.h"

#include <random>

static struct {
	std::mt19937 rng; //One rng for all
	std::uniform_real_distribution<double> real_distribution;
} rand_source;

void initRandom() {
	std::random_device rd;
	rand_source.rng.seed(rd());
	rand_source.real_distribution = std::uniform_real_distribution<double>(0.f, 1.f);
}

double getRandom() {
	return rand_source.real_distribution(rand_source.rng);
}

float getRandomFloat() {
	return static_cast<float>(rand_source.real_distribution(rand_source.rng));
}