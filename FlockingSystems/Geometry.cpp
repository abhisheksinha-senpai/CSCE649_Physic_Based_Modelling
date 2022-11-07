#include "Geometry.h"
#include "Boid.hpp"


void Geometry::draw_geometry(unsigned int& shaderPgm, unsigned int& vao, unsigned int& vbo)
{
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	float scale_fac = this->br;
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glm::vec3 scale = glm::vec3(scale_fac, scale_fac, scale_fac);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	model = glm::scale(model, scale);
	unsigned int modelLoc = glGetUniformLocation(shaderPgm, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glBufferData(GL_ARRAY_BUFFER, 216 * sizeof(float), body, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
	glEnableVertexAttribArray(1);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(shaderPgm, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Geometry::add_forces(std::vector<Boid> &bd, float coef)
{
#pragma omp parallel
	for (int i = 0; i < bd.size(); i++)
	{
		for (int j = 6; j < 216; j+=36)
		{
			glm::f32vec3 rel_pos = bd[i].position - position;
			float dist = glm::length(rel_pos);//glm::dot(rel_pos, faceNormals[(int)(j/36)]);
			if(dist>1.0f)
				bd[i].forces += -coef / (0.001f + abs(dist)* abs(dist)) * glm::normalize(rel_pos);
		}
	}
}

void Geometry::calculate_facenormals()
{
#pragma omp parallel
	for (int i = 6; i < 216; i += 36)
	{
		glm::vec3 vec1 = glm::vec3(body[i - 6] - body[i], body[i - 5] - body[i + 1], body[i - 4] - body[i + 2]);
		glm::vec3 vec2 = glm::vec3(body[i + 6] - body[i], body[i + 7] - body[i + 1], body[i + 8] - body[i + 2]);
		faceNormals[(int)(i / 36)] = glm::normalize(glm::cross(vec2, vec1));
		//std::cout << glm::to_string(faceNormals[(int)(i / 36)]) << " " << glm::to_string(vec1) << " " << glm::to_string(vec2) << std::endl;
	}
}


void Geometry::draw_lines(unsigned int& shaderPgm, unsigned int& vao, unsigned int& vbo, std::vector<Boid>& bd)
{
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glm::mat4 model = glm::mat4(1.0f);
	unsigned int modelLoc = glGetUniformLocation(shaderPgm, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	std::vector<float>arrow_data;
	for (auto a : bd)
	{
		arrow_data.push_back(a.position.x);
		arrow_data.push_back(a.position.y);
		arrow_data.push_back(a.position.z);

		arrow_data.push_back(0.56);
		arrow_data.push_back(0.35);
		arrow_data.push_back(0.87);

		arrow_data.push_back(position.x);
		arrow_data.push_back(position.y);
		arrow_data.push_back(position.z);

		arrow_data.push_back(0.56);
		arrow_data.push_back(0.35);
		arrow_data.push_back(0.87);
	}
	//std::cout << arrow_data.size() << std::endl;
	glBufferData(GL_ARRAY_BUFFER, arrow_data.size(), arrow_data.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
	glEnableVertexAttribArray(1);
	glDrawArrays(GL_LINES, 0, bd.size());

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Geometry::avoid_obstacles(std::vector<Boid>& bd, float tc)
{
#pragma omp parallel
	for (int i = 0; i < bd.size(); i++)
	{
		glm::f32vec3 vi = glm::normalize(bd[i].velocity);
		float s_close = glm::dot(this->position - bd[i].position , vi);
		float dc = glm::length(bd[i].velocity) * tc;
		if (s_close > 0 && s_close<dc)
		{
			glm::f32vec3 x_close = bd[i].position + s_close * vi;
			float d = glm::length(x_close - this->position);
			if (d <10.0f*br)
			{
				glm::f32vec3 v_t = glm::normalize(x_close - this->position);
				glm::f32vec3 x_t = this->position + 20.0f * br*v_t;
				float dt = glm::length(x_t - bd[i].position);
				float vt = glm::dot(x_t - bd[i].position, bd[i].velocity) / (dt+0.001f);
				float tt = dt / vt;
				glm::f32vec3 force = v_t * (2.0f * glm::length(glm::cross(bd[i].velocity, x_t - bd[i].position)) / (tt/10.0f));
				bd[i].forces +=100.0f*force;
				//d::cout << glm::to_string(force) << " | " << glm::length(force) << std::endl;
			}
		}
	}
}

void Geometry::repel(std::vector<Boid>& bd)
{
	for (int i = 0; i < bd.size(); i++)
	{
		float total = INT_MAX;
		int index = -1;
		glm::f32vec3 rel_pos_final;
		for (int j = 6; j < 216; j += 36)
		{
			glm::f32vec3 rel_pos = bd[i].position - br*glm::f32vec3(body[j], body[j+1], body[j+2]);
			float dist = abs(glm::dot(rel_pos, faceNormals[(int)(j / 36)]));
			if (dist < total)
			{
				total = dist;
				index = j/36;
				rel_pos_final = rel_pos;
			}
		}
		if (total <= 0.5f)
		{
			glm::f32vec3 v_n = glm::dot(bd[i].velocity, faceNormals[index])* faceNormals[index];
			glm::f32vec3 v_t = bd[i].velocity - v_n;
			bd[i].velocity = -(v_t+v_n);
		}
		//bd[i].forces += 5.0f / (0.001f + abs(total) * abs(total)) * glm::normalize(rel_pos_final);
	}
}

void Geometry::check_collision(std::vector<Boid>& bd)
{
	for (int i = 0; i < bd.size(); i++)
	{
		if (glm::length(position - bd[i].position) <  2.0*br)
			std::cout << "crashed" << std::endl;
	}
}
