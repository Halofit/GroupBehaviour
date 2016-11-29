#pragma once

#include <ctime>
#include <random>
#include <sstream>
#include <stdint.h>
#include <string>

inline std::string leftPad(std::string str, int len) {
	len++; // for the null character
	if (str.length() >= len) {
		str.resize(len);
		return str;
	} else {
		std::stringstream ss;
		for (size_t i = 0; i < (len - str.size()); i++) {
			ss << " ";
		}
		ss << str;
		return ss.str();
	}
}

struct Timer {
	std::clock_t begin;

	void start() {
		begin = std::clock();
	}

	double get() {
		clock_t end = clock();
		double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
		return elapsed_secs;
	}
};

struct Random {
	Random() {
		std::random_device rd;
		this->rng.seed(rd());
		this->double_distr = std::uniform_real_distribution<double>(0.f, 1.f);
		this->float_distr = std::uniform_real_distribution<float>(0.f, 1.f);
		this->int_distr = std::uniform_int_distribution<int>(INT_MIN, INT_MAX);
	}

	Random(unsigned int seed) {
		this->rng.seed(seed);
		this->double_distr = std::uniform_real_distribution<double>(0.f, 1.f);
		this->float_distr = std::uniform_real_distribution<float>(0.f, 1.f);
		this->int_distr = std::uniform_int_distribution<int>(INT_MIN, INT_MAX);
	}

	double getDouble() {
		return this->double_distr(this->rng);
	}

	float getFloat() {
		return this->float_distr(this->rng);
	}

	int getInt() {
		return this->int_distr(this->rng);
	}

	private:
	std::mt19937 rng;
	std::uniform_real_distribution<float> float_distr;
	std::uniform_real_distribution<double> double_distr;
	std::uniform_int_distribution<int> int_distr;
	// implementation
};
