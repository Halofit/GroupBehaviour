#pragma once

#include <cmath>
#include <cstdint>
#include <string>
#include <sstream>

static const double vectors_pi = 3.14159265358979323846;
static const double vectors_pi_f = static_cast<float>(vectors_pi);

template <typename T> struct Vec2Base {
	T x;
	T y;

	Vec2Base() : x(), y() {
	}

	Vec2Base(T x, T y) {
		this->x = x;
		this->y = y;
	}

	Vec2Base(float angle) {
		this->x = static_cast<T>(std::cosf(angle));
		this->y = static_cast<T>(std::sinf(angle));
	}

	float lengthSquared() {
		return (this->x * this->x) + (this->y * this->y);
	}

	float length() {
		return static_cast<float>(std::sqrt(this->lengthSquared()));
	}

	float angle() {
		auto a = std::atan2(y, x);
		if (a < 0.f) a += static_cast<float>(vectors_pi_f * 2.f);
		return a;
	}

	Vec2Base normalised() {
		float len = this->length();
		if (len == 0.0f) return Vec2Base(x, y);
		return Vec2Base(x / len, y / len);
	}

	Vec2Base rotate(float theta) {
		float px = this->x * std::cosf(theta) - this->y * std::sinf(theta);
		float py = this->x * std::sinf(theta) + this->y * std::cosf(theta);
		return Vec2Base(px, py);
	}

	friend Vec2Base operator-(Vec2Base a) {
		return Vec2Base(-a.x, -a.y);
	}

	friend Vec2Base operator+(Vec2Base a, Vec2Base b) {
		return Vec2Base(a.x + b.x, a.y + b.y);
	}

	Vec2Base operator+=(Vec2Base b) {
		x += b.x;
		y += b.y;
		return (*this);
	}

	friend Vec2Base operator-(Vec2Base a, Vec2Base b) {
		return Vec2Base(a.x - b.x, a.y - b.y);
	}

	Vec2Base operator-=(Vec2Base b) {
		x -= b.x;
		y -= b.y;
		return (*this);
	}

	friend Vec2Base operator*(Vec2Base v, T f) {
		return Vec2Base(v.x * f, v.y * f);
	}

	friend Vec2Base operator*(T f, Vec2Base v) {
		return Vec2Base(v.x * f, v.y * f);
	}

	Vec2Base operator*=(T f) {
		x *= f;
		y *= f;
		return (*this);
	}

	friend Vec2Base operator/(Vec2Base v, T f) {
		return Vec2Base(v.x / f, v.y / f);
	}


	Vec2Base operator/=(T f) {
		x /= f;
		y /= f;
		return (*this);
	}

	friend T operator*(Vec2Base a, Vec2Base b) {
		return a.x * b.x + a.y * b.y;
	}

	friend bool operator<(Vec2Base a, Vec2Base b) {
		return (a.x < b.x) && (a.y < b.y);
	}

	friend bool operator>(Vec2Base a, Vec2Base b) {
		return (a.x > b.x) && (a.y > b.y);
	}

	static Vec2Base pairMult(Vec2Base a, Vec2Base b) {
		return Vec2Base{a.x * b.x, a.y * b.y};
	}

	static Vec2Base pairDiv(Vec2Base a, Vec2Base b) {
		return Vec2Base{a.x / b.x, a.y / b.y};
	}

	std::string toString() {
		std::stringstream ss;
		ss << "(" << x << ", " << y << ")";
		return ss.str();
	}

	friend std::ostream& operator<<(std::ostream& str, const Vec2Base& vec) {
		return str << "(" << vec.x << ", " << vec.y << ")";
	}
};

using Vec2f = Vec2Base<float>;
using Vec2i = Vec2Base<int>;