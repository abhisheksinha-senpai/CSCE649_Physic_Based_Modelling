#pragma once
#include "Definitions.hpp"
#include "Particle.hpp"
#include "Asteroid.hpp"

class ParticleEmitter
{
private:
	
	std::queue<int> inactive_particles;
	glm::f32vec3 velocity;
	float outer_radius;
	float length = 0.1f;
	glm::f32vec3 orientation = glm::f32vec3{ 1.0f, 1.0f, 0.0f };
	int theta_ptr = 0;
	int height_ptr = 0;
	int frac_ptr = 0;
	int vel_ptr = 0;
	glm::f32mat3 Rot = glm::f32mat3(1.0f);
	float attract;
	float repel;

public:
	glm::f32vec3 position;
	std::vector<Particle> particles;
	float inner_radius;
	ParticleEmitter(glm::f32vec3 pos, glm::f32vec3 vel, float R, float r, float attract, float repel);
	void generate_particles(int count);
	void draw_particles(unsigned int& shaderPgm, unsigned int& vao, unsigned int& vbo);
	glm::f32vec3 generate_position();
	glm::f32vec3 generate_velocity(glm::f32vec3 pos);
	void update_particles(float delT);
	void EulerIntegration(float delT, std::vector<Asteroid>& Ast);
};

