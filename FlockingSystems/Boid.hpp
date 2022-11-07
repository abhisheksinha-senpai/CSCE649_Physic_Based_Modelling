#pragma once
#include "Definitions.hpp"

struct Boid
{
	glm::f32vec3 position;
	glm::f32vec3 past_position;
	glm::f32vec3 velocity;
	glm::f32vec3 color;
	glm::f32vec3 accelration;
	glm::f32vec3 forces;
	bool active;
	float life = 100.0f;
	float mass;

	Boid(glm::f32vec3 pos, glm::f32vec3 vel, float m, bool act, float lf, glm::f32vec3 col):position(pos), velocity(vel), mass(m), active(act), life(lf), color(col), forces(0.0f)
	{
		past_position = pos;
		forces = glm::f32vec3(0.0f, 0.0f, 0.0f);
	};
	Boid() {};
	~Boid() {};
};

class FlockingSystem
{
private:
	int theta_ptr=0;
	int height_ptr=0;
	int frac_ptr=0;
	int vel_ptr = 0;

	glm::f32vec3 triangle_body[18] = { glm::f32vec3(0.0f, 1.0f, 0.0f), glm::f32vec3(0.5f, 0.0f, 0.0f), glm::f32vec3(0.0f, 0.0f, 0.2f), 
									   glm::f32vec3(0.0f, 1.0f, 0.0f), glm::f32vec3(-0.5f, 0.0f, 0.0f), glm::f32vec3(0.0f, 0.0f, 0.2f), 
									   glm::f32vec3(0.0f, 1.0f, 0.0f), glm::f32vec3(0.5f, 0.0f, 0.0f), glm::f32vec3(0.0f, 0.0f, -0.2f), 
									   glm::f32vec3(0.0f, 1.0f, 0.0f), glm::f32vec3(-0.5f, 0.0f, 0.0f), glm::f32vec3(0.0f, 0.0f, -0.2f),
									   glm::f32vec3(-0.5f, 0.0f, 0.0f), glm::f32vec3(0.0f, 0.0f, 0.2f), glm::f32vec3(0.5f, 0.0f, 0.0f),
									   glm::f32vec3(-0.5f, 0.0f, 0.0f), glm::f32vec3(0.0f, 0.0f, -0.2f), glm::f32vec3(0.5f, 0.0f, 0.0f)};

	glm::f32vec3 position;
	glm::f32vec3 mean_vel;
public:
	std::vector<Boid>BoidSystem;

	void add_forces();
	float calculate_field_of_vision(int i, int j);
	float calculate_radius_of_influence(int i, int j);
	void draw_boids(unsigned int& shaderPgm, unsigned int& vao, unsigned int& vbo);
	void calculate_new_state_vector(float delT);
	void generate_particles(int count);
	glm::f32vec3 generate_position();
	glm::f32vec3 generate_velocity();
	FlockingSystem(glm::f32vec3 position, glm::f32vec3 mean_vel);

	friend class Geomtry;
};