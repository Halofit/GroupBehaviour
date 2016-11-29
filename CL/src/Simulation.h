#pragma once
#include <vector>
#include <iostream>
#include <algorithm>
#include "Vectors.h"
#include "Random.h"
#include "Error.h"

using std::cout;

struct Agent {
	Agent(float size_d, Vec2f loc) {
		diameter = size_d;
		location = loc;
	}


	float diameter;
	float mass = 80;
	Vec2f velocity = 0;
	Vec2f direction{1.f, 0.f};
	Vec2f location;

	float desired_velocity = 5;

	Vec2f accumulator;

};


struct Wall {
	Wall(Vec2f locA, Vec2f locB) :
		locationA(locA),
		locationB(locB)
	{
	}

	const Vec2f locationA;
	const Vec2f locationB;

	// Return minimum distance between line segment vw and point p
	float distanceFrom(Vec2f p) const {

		//TODO rewrite Stack overflow code
		const Vec2f v = locationA;
		const Vec2f w = locationB;
		
		const float l2 = (v - w).lengthSquared(); // i.e. |w-v|^2 -  avoid a sqrt
		//if (l2 == 0.0) return distance(p, v);   // v == w case

		// Consider the line extending the segment, parameterized as v + t (w - v).
		// We find projection of point p onto the line. 
		// It falls where t = [(p-v) . (w-v)] / |w-v|^2
		// We clamp t from [0,1] to handle points outside the segment vw. -> Clamp with max(0,min(1,x))

		const float t = std::max(0.f, std::min(1.f, (p - v) * (w - v) / l2 ));
		const Vec2f projection = v + t * (w - v);  // Projection falls on the segment
		return (p - projection).length();
	}

	float distabceFromUnboundLine(Vec2f p0) const{

		//TODO this is just the hight of the p0 p1 p2 triangle -> need to add bounds checking see above
		const Vec2f p1 = locationA;
		const Vec2f p2 = locationB;

		float numer = abs((p2.y - p1.y)*p0.x - (p2.x - p1.x)*p0.y + p2.x*p1.y - p2.y*p1.x);
		float denom = sqrt((p2.y - p1.y)*(p2.y - p1.y) + (p2.x - p1.x)*(p2.x - p1.x));
		float distance_to_line = numer / denom;

		return distance_to_line; 
	}
};

static const float max_velocity = 5.f;

static const float agent_acceleration_time = 0.5;
static const float A_i = 2e3f;
static const float B_i = 0.08f;

static const float k = 1.2e5;
static const float kappa = 2.4e5;

struct Simulation {
	std::vector<Agent> agents;
	std::vector<Wall> walls;


	Simulation() {
	}

	void clear() {
		agents.clear();
		walls.clear();
	}

	void clearAgents() {
		agents.clear();
	}

	std::vector<Agent>* getAgents() {
		return &agents;
	}


	std::vector<Wall>* getWalls() {
		return &walls;
	}


	void addAgent(Vec2f loc) {
		float diameter = 0.5f + (2.f * getRandomFloat() / 10.f);
		agents.push_back(Agent(diameter, loc));
	}

	void addWall(Vec2f locA, Vec2f locB) {
		walls.push_back(Wall(locA, locB));
	}


	static float helper_g_function(float dist) {
		return std::max(dist, 0.f);
	};

	Vec2f force(const Agent& ai, const Agent& aj) const {
		auto g = helper_g_function;

		float d_ij = (ai.location - aj.location).length();
		Vec2f n_ij = (ai.location - aj.location).normalised();
		float r_ij = (ai.diameter + aj.diameter)/2;
		Vec2f t_ij(-n_ij.y, n_ij.x);
		Vec2f delta_v_ji = (aj.velocity - ai.velocity) * t_ij;

		float dd = (r_ij - d_ij);

		Vec2f f_ij = (A_i * exp(dd / B_i) + k * g(dd)) * n_ij + ( kappa*g(dd) * delta_v_ji * t_ij );
		return f_ij;
	}

	Vec2f force(const Agent& ai, const Wall& w) const {
		auto g = helper_g_function;

		float d_iw = w.distanceFrom(ai.location);

		Vec2f t_iw = (w.locationB - w.locationA).normalised(); //TODO does this point in the right direction?
		Vec2f n_iw(-t_iw.y, t_iw.x);

		
		float dd = (ai.diameter/2 - d_iw);

		Vec2f f_ij = (A_i * exp(dd / B_i) + k * g(dd)) * n_iw - kappa*g(dd) * (ai.velocity * t_iw) * t_iw;
		return f_ij;
	}

	void tick(float delta_t) {
		//First term
		for (auto& a : agents) {
			a.accumulator = a.mass * ((a.desired_velocity * a.direction - a.velocity) / agent_acceleration_time);
		}

		

		//Second term
		for (auto& a : agents) {
			for (auto& a2 : agents) {
				if (&a != &a2) {
					a.accumulator += force(a, a2);
				}
			}
		}

		//Third term
		for (auto& a : agents) {
			for (auto& w : walls) {
				a.accumulator += force(a, w);
			}
		}


		//Apply
		for (auto& a : agents) {
			Vec2f velocityChange = (a.accumulator/a.mass)*delta_t;
			if (velocityChange.lengthSquared() > max_velocity*max_velocity) {
				velocityChange = velocityChange.normalised() * max_velocity;
			}

			a.velocity += velocityChange;
			a.location += a.velocity * delta_t;
		}
	}
};