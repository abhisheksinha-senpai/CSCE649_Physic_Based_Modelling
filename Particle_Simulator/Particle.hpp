#pragma once
#include "Definitions.hpp"

struct Particle
{
	glm::f32vec3 position;
	glm::f32vec3 past_position;
	glm::f32vec3 velocity;
	glm::f32vec3 color;
	glm::f32vec3 forces;
	bool active;
	float life = 100.0f;
	float mass;

	Particle(glm::f32vec3 pos, glm::f32vec3 vel, float m, bool act, float lf, glm::f32vec3 col):position(pos), velocity(vel), mass(m), active(act), life(lf), color(col), forces(0.0f)
	{
		past_position = pos;
		forces = glm::f32vec3(0.0f, 0.0f, 0.0f);
	};
	Particle() {};
	~Particle() {};
};