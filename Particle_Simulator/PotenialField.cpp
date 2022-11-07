#include "PotenialField.h"

void PotenialField::add_force(std::vector<Particle>&particles)
{
	for (auto &particle : particles)
	{
		glm::f32vec3 r = particle.position - this->position;
		float distance = glm::length(r);
		float coef = (1.0f / glm::pow(distance, 2));
		glm::f32vec3 force = coef * (r / distance);
		if (distance < this->radius)
		{
			coef = (1.0f / glm::pow(distance, 4));
			particle.forces += this->Ga2 * force;
		}
		particle.forces += -this->Ga1 * force;
	}
}