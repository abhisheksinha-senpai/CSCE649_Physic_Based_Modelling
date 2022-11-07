#include "Definitions.h"

void init_objetcs(int n)
{
	for (int i = 0; i < n; i++)
	{
		ball_struct object;
		object.position = glm::vec3(glm::linearRand(-0.5f, 0.5f), glm::linearRand(-0.5f, 0.5f), glm::linearRand(-0.5f, 0.5f));

		object.velocity = glm::vec3(glm::linearRand(-5.0f, 1.0f), glm::linearRand(-1.0f, 5.0f), glm::linearRand(-1.0f, 5.0f));
		object.m = glm::linearRand(0.5f, 5.0f);
		all_objects.push_back(object);
		collision.push_back(std::vector<bool>(n, false));
	}
}

void draw_balls(unsigned int &shaderPgm, unsigned int &ballbuffer)
{
	unsigned int modelLoc = glGetUniformLocation(shaderPgm, "model");
	glm::mat4 model = glm::mat4(1.0f);
	
	glBindBuffer(GL_ARRAY_BUFFER, ballbuffer);
	glBufferData(GL_ARRAY_BUFFER, 144 * sizeof(float), ball, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// render the ball
	glBindBuffer(GL_ARRAY_BUFFER, ballbuffer);
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	for (auto object : all_objects)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(object.position[0], object.position[1], object.position[2]));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 24);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}