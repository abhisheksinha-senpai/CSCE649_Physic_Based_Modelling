#include "ParticleEmitter.h"

extern std::vector<float> gaussian_dist;
extern std::vector<float> uniform_dist;

static std::normal_distribution<float> distribution_mass(5.0f, 1.0f);
std::vector<float>particle_vbo_data(MAX_PARTICLES* (3 + 4), 0.0f);

Particle p = Particle();

ParticleEmitter::ParticleEmitter(glm::f32vec3 pos, glm::f32vec3 vel, float R, float r, float attract, float repel) : position(pos), velocity(vel), outer_radius(R), inner_radius(r), attract(attract), repel(repel)
{

	glm::f32vec3 some_vec = glm::cross(glm::vec3(0, 1, 0),glm::normalize(this->orientation));
	float s = glm::length(some_vec);
	float c = glm::dot(glm::vec3(0, 1, 0), this->orientation);

	glm::f32mat3 Vx = glm::f32mat3(glm::f32vec3(0.0f, -some_vec.z, some_vec.y), glm::f32vec3(some_vec.z, 0.0f, -some_vec.x), glm::f32vec3(-some_vec.y, some_vec.x, 0.0f));

	Rot = glm::f32mat3(1.0f) + Vx + Vx * Vx * ((1.0f - c) / (s * s));
}

void ParticleEmitter::generate_particles(int count)
{
	static std::default_random_engine generator_;
	static std::normal_distribution<float> distribution_mass(5.0f, 1.0f);
	
	int curr_pumped = 0;
	glm::f32vec3 pos, vel;

	for (int i = 0; i < count; i++)
	{
		pos = generate_position();
		vel = generate_velocity(pos);
		glm::f32vec3 color_particle = glm::f32vec3(pos.x * pos.x, pos.y * pos.y, pos.z * pos.z);
		color_particle = glm::normalize(color_particle);
		float life = gaussian_dist[theta_ptr++] * PARTICLE_LIFE + PARTICLE_LIFE/3.0f;
		if (this->particles.size() < MAX_PARTICLES)
		{
			total_particles ++;
			this->particles.push_back(Particle{ pos, vel, distribution_mass(generator_), true, life, color_particle });
		}
		else
		{
			if (this->inactive_particles.size()>0)
			{
				int id = this->inactive_particles.front();
				this->particles[id].position = pos;
				this->particles[id].velocity = vel;
				color_particle = glm::f32vec3(pos.x * pos.x, pos.y * pos.y, pos.z * pos.z);
				color_particle = glm::normalize(color_particle);
				this->particles[id].color = color_particle;
				this->particles[id].active = true;
				this->particles[id].life = life;
				this->inactive_particles.pop();
			}
		}
	}
	
}

void ParticleEmitter::draw_particles(unsigned int &shaderPgm, unsigned int& vao, unsigned int& vbo)
{
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	fill(particle_vbo_data.begin(), particle_vbo_data.end(), 0.0f);
	int k = 0;
	for (auto & p : this->particles)
	{
		if (p.active)
		{
			particle_vbo_data[k++] = p.position[0];
			particle_vbo_data[k++] = p.position[1];
			particle_vbo_data[k++] = p.position[2];

			particle_vbo_data[k++] = p.color[0];
			particle_vbo_data[k++] = p.color[1];
			particle_vbo_data[k++] = p.color[2];
		}
		//particle_vbo_data[k++] = p.color[3];
	}
	glm::mat4 model = glm::mat4(1.0f);
	int modelLoc = glGetUniformLocation(shaderPgm, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glPointSize(2);
	glBufferData(GL_ARRAY_BUFFER, k*sizeof(float), particle_vbo_data.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(shaderPgm, "aPos"), 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(glGetAttribLocation(shaderPgm, "aColor"), 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
	glEnableVertexAttribArray(1);
	glDrawArrays(GL_POINTS, 0, (int)k/7);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

glm::f32vec3 ParticleEmitter::generate_position()
{
	float theta = uniform_dist[theta_ptr++] * (glm::pi<float>() - -(glm::pi<float>())) - (glm::pi<float>());
	theta_ptr = theta_ptr % (3 * MAX_PARTICLES);
	float y = uniform_dist[height_ptr++] * (0.2f - -(0.2f)) + (-0.2f);
	height_ptr = height_ptr % (3 * MAX_PARTICLES);
	float r = glm::sqrt(1.0f - y * y);
	glm::f32vec3 u_cap = { r * glm::cos(theta), y, -r * glm::sin(theta) };
	u_cap = glm::normalize(u_cap);

	
	float scale = uniform_dist[frac_ptr++]*(outer_radius - inner_radius) + 1.25f*inner_radius;
	frac_ptr = frac_ptr % (3 * MAX_PARTICLES);
	glm::f32vec3 v = this->position + Rot*u_cap ;
	return v;
}

glm::f32vec3 ParticleEmitter::generate_velocity(glm::f32vec3 pos)
{
	glm::f32vec3 u_z = Rot*glm::normalize(this->orientation);
	glm::f32vec3 loc_pos = (pos - this->position);
	glm::f32vec3 temp = loc_pos - glm::dot(loc_pos, u_z) * u_z;
	temp = glm::normalize(temp);
	loc_pos = glm::normalize(loc_pos);
	glm::f32vec3 required_dir = glm::cross(temp, u_z);
	required_dir = Rot*glm::normalize(required_dir);
	glm::f32vec3 v = required_dir * 1.0f*glm::sqrt(glm::length(loc_pos));
	//std::cout << glm::length(v)<<std::endl;
	return v;
}

void ParticleEmitter::update_particles(float delT)
{
	for (int i=0;i<this->particles.size();i++)
	{
		
		this->particles[i].life -= delT;
		this->particles[i].forces = glm::f32vec3(0.0f, 0.0f, 0.0f);
		if (this->particles[i].active && this->particles[i].life <= 0.0f)
		{
			this->inactive_particles.push(i);
			this->particles[i].active = false;
		}
	}
	//std::cout << this->inactive_particles.size() << " | " << this->particles.size() << std::endl;
}

void ParticleEmitter::EulerIntegration(float delT, std::vector<Asteroid>& Ast)
{
#pragma omp parallel
	for (int i = 0; i < this->particles.size(); i++)
	{
		this->particles[i].past_position = this->particles[i].position;
		this->particles[i].velocity += delT * this->particles[i].forces;
		this->particles[i].position += delT * this->particles[i].velocity;
		for (auto& a : Ast)
		{
			for (int j = 0; j < 144; j += 18)
			{
				glm::f32vec3 v1 = a.position + a.br * glm::f32vec3(a.body[j], a.body[j + 1], a.body[j + 2]);
				glm::f32vec3 v2 = a.position + a.br * glm::f32vec3(a.body[j + 6], a.body[j + 7], a.body[j + 8]);
				glm::f32vec3 v3 = a.position + a.br * glm::f32vec3(a.body[j + 12], a.body[j + 13], a.body[j + 14]);
				//std::cout << glm::to_string(v1) << " " << glm::to_string(v2) << " " << glm::to_string(v3) << std::endl;
				glm::f32vec3 u = v2 - v1;
				glm::f32vec3 v = v3 - v1;
				glm::f32vec3 n = glm::cross(u, v);
				glm::f32vec3 w = this->particles[i].past_position - v1;
				float gamma = glm::dot(glm::cross(u, w), n) / glm::dot(n, n);
				float beta = glm::dot(glm::cross(w, v), n) / glm::dot(n, n);
				float alpha = 1.0f - gamma - beta;

				bool cond1 = (alpha <= 1.0f && alpha >= 0.0f);
				bool cond2 = (beta <= 1.0f && beta >= 0.0f);
				bool cond3 = (gamma <= 1.0f && gamma >= 0.0f);
				//std::cout << cond1 << " | " << cond2 << " | " << cond3 << " | "<<i<<std::endl;

				n = glm::normalize(n);
				glm::f32vec3 xr2 = this->particles[i].position - v1;
				float dist1 = glm::dot(w, n);
				float dist2 = glm::dot(xr2, n);
				//std::cout << dist1 << " | " << dist2 << std::endl;
				if (dist1 / dist2 < 0.0f && cond1 && cond2 && cond3)
				{
					//std::cout << "here" << std::endl;
					this->particles[i].position = this->particles[i].position + (1.0f + coef_res) * n * dist2+ dist1 *n/abs(dist1)*3.0f*dist_tol;
					glm::f32vec3 vn = glm::dot(this->particles[i].velocity, n) * n;
					glm::f32vec3 vt = this->particles[i].velocity - vn;
					this->particles[i].velocity = -1.0f * coef_res * vn + (1.0f - coef_fric) * vt;
					break;
				}
			}
		}
	}
}