#pragma once

#include "Definitions.hpp"
#include "Particle.hpp"

class PotenialField
{
private:
	float radius;
	float Ga1;
	float Ga2;
public:
	glm::f32vec3 position;
	PotenialField(float Ga1, float Ga2, float rad, glm::f32vec3 init_pos) :Ga1(Ga1), Ga2(Ga2), radius(rad), position(init_pos) {};
	void add_force(std::vector<Particle> &particles);
};

