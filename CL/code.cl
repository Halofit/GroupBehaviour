#include <math.h>


typedef struct {
	float x;
	float y;
} Vec2f;

struct Agent {
	float diameter;
	float mass = 80;
	Vec2f velocity = 0;
	Vec2f direction{1.f, 0.f};
	Vec2f location;

	float desired_velocity = 5;

	Vec2f accumulator;
};


struct Agent createAgent(float size_d, Vec2f loc) {
	Agent a;
	//Constants
	a.mass = 80;
	a.velocity.x = 0;
	a.velocity.y = 0;
	a.direction.x = 1.f;
	a.direction.y = 0.f;
	a.desired_velocity = 5;
	
	a.diameter = size_d;
	a.location = loc;	
	
	return a;
}


typedef struct {
	Vec2f locationA;
	Vec2f locationB;
} Wall;

Wall createWall(Vec2f locA, Vec2f locB) {
	Wall w;
	w.locationA = locA;
	w.locationB = locB;
	return w;
}

// Return minimum distance between line segment vw and point p
Vec2f getClosestPoint(Vec2f p) const {

	//TODO rewrite Stack overflow code
	const Vec2f v = locationA;
	const Vec2f w = locationB;

	const float l2 = (v - w).lengthSquared(); // i.e. |w-v|^2 -  avoid a sqrt
	//if (l2 == 0.0) return distance(p, v);   // v == w case

	const float t = max(0.f, min(1.f, (p - v) * (w - v) / l2));
	const Vec2f projection = v + t * (w - v); // Projection falls on the segment
	return projection;
}

__kernel void interactions(__global Agent* agents,
						   __global int num_agents,
						   __global Wall* walls,
						   __global int num_walls)
{
		
}



