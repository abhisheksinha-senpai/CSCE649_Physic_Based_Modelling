#pragma once
#include "Definitions.hpp"

class Asteroid
{
    unsigned int ast_array_buffer, ast_object_buffer;
public:
	glm::f32vec3 position;
	float br;
	float body[144] = {
		// positions         // colors
	 1.0f,  0.0f,  0.0f,   1.0f, 1.0f, 1.0f, // triangle 1
	  0.0f, 1.0f,  0.0f,   1.0f, 1.0f, 1.0f,
	  0.0f,  0.0f, 1.0f,   1.0f, 1.0f, 1.0f,
	  0.0f, 1.0f,  0.0f,   1.0f, 1.0f, 1.0f, // triangle 2
	-1.0f,  0.0f,  0.0f,   1.0f, 1.0f, 1.0f,
	  0.0f,  0.0f, 1.0f,   1.0f, 1.0f, 1.0f,
	-1.0f,  0.0f,  0.0f,   1.0f, 1.0f, 1.0f, // triangle 3
	  0.0f,-1.0f,  0.0f,   1.0f, 1.0f, 1.0f,
	  0.0f,  0.0f, 1.0f,   1.0f, 1.0f, 1.0f,
	  0.0f,-1.0f,  0.0f,   1.0f, 1.0f, 1.0f, // triangle 4
	 1.0f,  0.0f,  0.0f,   1.0f, 1.0f, 1.0f,
	  0.0f,  0.0f, 1.0f,   1.0f, 1.0f, 1.0f,
	 1.0f,  0.0f,  0.0f,   1.0f, 1.0f, 1.0f, // triangle 5
	  0.0f,-1.0f,  0.0f,   1.0f, 1.0f, 1.0f,
	  0.0f,  0.0f,-1.0f,   1.0f, 1.0f, 1.0f,
	  0.0f,-1.0f,  0.0f,   1.0f, 1.0f, 1.0f, // triangle 6
	-1.0f,  0.0f,  0.0f,   1.0f, 1.0f, 1.0f,
	  0.0f,  0.0f,-1.0f,   1.0f, 1.0f, 1.0f,
	-1.0f,  0.0f,  0.0f,   1.0f, 1.0f, 1.0f, // triangle 7
	  0.0f, 1.0f,  0.0f,   1.0f, 1.0f, 1.0f,
	  0.0f,  0.0f,-1.0f,   1.0f, 1.0f, 1.0f,
	  0.0f, 1.0f,  0.0f,   1.0f, 1.0f, 1.0f, // triangle 8
	 1.0f,  0.0f,  0.0f,   1.0f, 1.0f, 1.0f,
	  0.0f,  0.0f,-1.0f,   1.0f, 1.0f, 1.0f,
	};
	Asteroid(float br, glm::f32vec3 pos);
	void draw_asteroid(unsigned int& shaderPgm, unsigned int& vao, unsigned int& vbo);
};

