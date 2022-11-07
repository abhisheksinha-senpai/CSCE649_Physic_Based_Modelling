#include "Asteroid.hpp"

Asteroid::Asteroid(float br, glm::f32vec3 pos) : br(br), position(pos)
{}

void Asteroid::draw_asteroid(unsigned int& shaderPgm, unsigned int& vao, unsigned int& vbo)
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
	glBufferData(GL_ARRAY_BUFFER, 144 * sizeof(float), body, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
	glEnableVertexAttribArray(1);
	glDrawArrays(GL_TRIANGLES, 0, 24);
	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(shaderPgm, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}