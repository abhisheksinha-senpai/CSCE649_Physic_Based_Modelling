#include "Definitions.hpp"
#include "Boid.hpp"

extern std::vector<float> gaussian_dist;
extern std::vector<float> uniform_dist;

extern float coef_avoid;
extern float coef_vel_match;
extern float coef_center;

extern float frontal_vision;
extern float peripheral_vision;

extern float inner_rad;
extern float outer_rad;

extern float banking_multiplier;

float running_angle = 0;

std::vector<float>particle_vbo_data(MAX_PARTICLES* (3 + 4), 0.0f);
std::vector<float>particle_vbo_data1(MAX_PARTICLES* (3 + 4), 0.0f);

FlockingSystem::FlockingSystem(glm::f32vec3 pos, glm::f32vec3 vel)
{
	mean_vel = vel;
	position = pos;
}

void FlockingSystem::add_forces()
{
#pragma omp parallel
	for (int i=0;i< BoidSystem.size();i++)
	{
		float dist, w1, w2;
		glm::f32vec3 direction;
		dist = 0;
		direction = glm::f32vec3(0.0f, 0.0f, 0.0f);
		for (int j = 0; j < BoidSystem.size(); j++)
		{
			if (i != j)
			{
				w1 = 1.0f;//calculate_radius_of_influence(i, j);
				w2 = 1.0f;// calculate_field_of_vision(i, j);
				direction = BoidSystem[j].position - BoidSystem[i].position;
				dist = glm::length(direction);
				BoidSystem[i].forces += w1 * w2 * -coef_avoid * direction / (dist+0.01f);
				BoidSystem[i].forces += w1 * w2 * coef_center * direction;
				BoidSystem[i].forces += w1 * w2 * coef_vel_match * (BoidSystem[j].velocity - BoidSystem[i].velocity);
			}
		}
		//std::cout << glm::to_string(BoidSystem[i].forces) << "  " << w1 <<"   " << w2 << std::endl;
	}
}


float FlockingSystem::calculate_radius_of_influence(int i, int j)
{
	float dist = glm::length(BoidSystem[j].position - BoidSystem[i].position);
	if (dist < inner_rad)
		return 1.0f;
	else if (dist > inner_rad && dist <= outer_rad)
	{
		float num = outer_rad - dist;
		float din = outer_rad - inner_rad;
		return num / din;
	}
	else
		return 0.0f;
}

float FlockingSystem::calculate_field_of_vision(int i, int j)
{
	glm::f32vec3 rel_pos = BoidSystem[j].position - BoidSystem[i].position;
	float theta = glm::acos(glm::dot(BoidSystem[i].velocity, rel_pos)/(glm::length(BoidSystem[i].velocity)* glm::length(rel_pos)));
	if (abs(theta) <= frontal_vision / 2)
		return 1.0f;
	else if (abs(theta) > frontal_vision / 2 && abs(theta) <= peripheral_vision / 2)
	{
		float num = peripheral_vision / 2 - abs(theta);
		float din = peripheral_vision / 2 - frontal_vision / 2;
		return num / din;
	}
	else
		return 0.0f;
}

bool first = true;
void FlockingSystem::draw_boids(unsigned int& shaderPgm, unsigned int& vao, unsigned int& vbo)
{
	fill(particle_vbo_data.begin(), particle_vbo_data.end(), 0.0f);
	fill(particle_vbo_data1.begin(), particle_vbo_data1.end(), 0.0f);
	int k = 0, ui=0;
	glm::f32mat3 Rot = glm::f32mat3(1.0f);
	glm::f32mat3 Bank = glm::f32mat3(1.0f);
	float scale_fac = 0.2f;
	glm::vec3 scale = glm::vec3(scale_fac, scale_fac, scale_fac);
	//model = glm::scale(model, scale);
	for (auto& p : BoidSystem)
	{
		//std::cout << glm::to_string(p.position) << std::endl;
		Rot = glm::f32mat3(1.0f);
		glm::f32vec3 some_vec = glm::cross(glm::normalize(p.velocity), glm::vec3(0, 1, 0));
		float s = glm::length(some_vec);
		float c = glm::dot(glm::vec3(0, 1, 0), glm::normalize(p.velocity));
		glm::f32mat3 Vx = glm::f32mat3(glm::f32vec3(0.0f, -some_vec.z, some_vec.y), glm::f32vec3(some_vec.z, 0.0f, -some_vec.x), glm::f32vec3(-some_vec.y, some_vec.x, 0.0f));
		Rot = glm::f32mat3(1.0f) + Vx + Vx * Vx * ((1.0f - c) / (s * s));
		if (glm::length(p.accelration) > 0.0001)
		{
			glm::f32vec3 ux = glm::normalize(p.velocity);
			glm::f32vec3 uy = glm::cross(p.velocity, p.accelration);
			uy = glm::normalize(uy);
			glm::f32vec3 uz = glm::cross(ux, uy);
			glm::f32vec3 av = glm::dot(p.accelration, ux) * ux;
			glm::f32vec3 at = p.accelration - av;

			float angle = glm::atan(-glm::dot(at, uz) * banking_multiplier);

			Bank = glm::f32mat3({ glm::cos(angle), 0, glm::sin(angle) }, { 0,1,0 }, { -glm::sin(angle), 0, glm::cos(angle) });
		}
		glm::f32vec3 pos[18];
		for (int i = 0; i < 18; i++)
		{
			pos[i] = p.position + scale_fac * Rot * Bank * triangle_body[i];
		}
		//std::cout << glm::to_string(p.position) << std::endl;
		//if (p.active)
		for(int j=0;j<18;j++)
		{
			particle_vbo_data[k++] = pos[j].x;
			particle_vbo_data[k++] = pos[j].y;
			particle_vbo_data[k++] = pos[j].z;

			particle_vbo_data[k++] = p.color[0];
			particle_vbo_data[k++] = p.color[1];
			particle_vbo_data[k++] = p.color[2];
		}
		for (int j = 0; j < 18; j++)
		{
			particle_vbo_data1[ui++] = pos[j].x;
			particle_vbo_data1[ui++] = pos[j].y;
			particle_vbo_data1[ui++] = pos[j].z;

			particle_vbo_data1[ui++] = 0.33;
			particle_vbo_data1[ui++] = 0.13;
			particle_vbo_data1[ui++] = 0.01;
		}
		//particle_vbo_data[k++] = p.color[3];
	}

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glm::mat4 model = glm::mat4(1.0f);
	int modelLoc = glGetUniformLocation(shaderPgm, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//glPointSize(10);
	glBufferData(GL_ARRAY_BUFFER, k * sizeof(float), particle_vbo_data.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(shaderPgm, "aPos"), 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(glGetAttribLocation(shaderPgm, "aColor"), 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
	glEnableVertexAttribArray(1);
	glDrawArrays(GL_TRIANGLES, 0, (int)k / 7);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	 model = glm::mat4(1.0f);
	 modelLoc = glGetUniformLocation(shaderPgm, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//glPointSize(10);
	glBufferData(GL_ARRAY_BUFFER, ui * sizeof(float), particle_vbo_data1.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(shaderPgm, "aPos"), 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(glGetAttribLocation(shaderPgm, "aColor"), 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
	glEnableVertexAttribArray(1);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(3.0f);
	glDrawArrays(GL_TRIANGLES, 0, (int)k / 7);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void FlockingSystem::calculate_new_state_vector(float delT)
{
#pragma omp parallel
	for (int i = 0; i < this->BoidSystem.size(); i++)
	{
		this->BoidSystem[i].past_position = this->BoidSystem[i].position;
		this->BoidSystem[i].velocity += delT * this->BoidSystem[i].forces;
		if (this->BoidSystem[i].velocity.x > 2.0f)
			this->BoidSystem[i].velocity.x = 2.0f;
		else if(this->BoidSystem[i].velocity.x < -2.0f)
			this->BoidSystem[i].velocity.x = -2.0f;

		if (this->BoidSystem[i].velocity.y > 2.0f)
			this->BoidSystem[i].velocity.y = 2.0f;
		else if (this->BoidSystem[i].velocity.y < -2.0f)
			this->BoidSystem[i].velocity.y = -2.0f;


		if (this->BoidSystem[i].velocity.z > 2.0f)
			this->BoidSystem[i].velocity.z = 2.0f;
		else if (this->BoidSystem[i].velocity.z < -2.0f)
			this->BoidSystem[i].velocity.z = -2.0f;

		this->BoidSystem[i].position += delT * this->BoidSystem[i].velocity;
		this->BoidSystem[i].accelration = this->BoidSystem[i].forces;
		this->BoidSystem[i].forces = glm::f32vec3(0.0f, 0.0f, 0.0f);
		//std::cout << glm::to_string(this->BoidSystem[i].past_position) << " " << glm::to_string(this->BoidSystem[i].position) << std::endl;
	}
}

void FlockingSystem::generate_particles(int count)
{
	static std::default_random_engine generator_;
	static std::normal_distribution<float> distribution_mass(5.0f, 1.0f);

	int curr_pumped = 0;
	glm::f32vec3 pos, vel;

	for (int i = 0; i < count; i++)
	{
		pos = generate_position();
		vel = generate_velocity();
		glm::f32vec3 color_particle = glm::f32vec3(pos.x * pos.x, pos.y * pos.y, pos.z * pos.z);
		color_particle = glm::normalize(color_particle);
		float life = gaussian_dist[theta_ptr++] * PARTICLE_LIFE + PARTICLE_LIFE / 3.0f;
		this->BoidSystem.push_back(Boid{ pos, vel, distribution_mass(generator_), true, life, color_particle });
	}
}

glm::f32vec3 FlockingSystem::generate_position()
{
	float theta = uniform_dist[theta_ptr++] * (glm::pi<float>() - -(glm::pi<float>())) - (glm::pi<float>());
	theta_ptr = theta_ptr % (3 * MAX_PARTICLES);
	float y = uniform_dist[height_ptr++] * (0.5f - (-0.5f)) + (-0.5f);
	height_ptr = height_ptr % (3 * MAX_PARTICLES);
	float r = glm::sqrt(1.0f - y * y);
	glm::f32vec3 u_cap = { r * glm::cos(theta), y, -r * glm::sin(theta) };
	u_cap = glm::normalize(u_cap);
	glm::f32vec3 v = this->position + u_cap;
	return  v;
}

glm::f32vec3 FlockingSystem::generate_velocity()
{
	glm::f32vec3 v = gaussian_dist[vel_ptr++] * mean_vel / 3.0f + mean_vel;
	vel_ptr = vel_ptr % (3 * MAX_PARTICLES);
	return v;
}

