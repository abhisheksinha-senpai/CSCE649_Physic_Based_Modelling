#pragma once
#include "Definitions.hpp"
#include "Boid.hpp"

class Geometry
{
private:
	float br;

	float body[216] = {
        // positions         // colors
         1.0f, -1.0f, -1.0f,  0.0f, 0.0f, 1.0f, // bottom
         1.0f,  1.0f, -1.0f,  0.0f, 0.0f, 1.0f,
        -1.0f,  1.0f, -1.0f,  0.0f, 0.0f, 1.0f,
        -1.0f,  1.0f, -1.0f,  0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, 1.0f,
         1.0f, -1.0f, -1.0f,  0.0f, 0.0f, 1.0f,
         1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top
         1.0f, -1.0f,  1.0f,  0.0f, 1.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 1.0f,
         1.0f, -1.0f,  1.0f,  0.0f, 1.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,  0.0f, 1.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 1.0f, // left
        -1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
         1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
         1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 1.0f,
         1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f, // right
        -1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 0.0f,
         1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 0.0f,
         1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 0.0f,
        -1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 0.0f, // back
        -1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 0.0f,
        -1.0f,  1.0f, -1.0f,  1.0f, 0.0f, 0.0f,
        -1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 0.0f,
        -1.0f,  1.0f, -1.0f,  1.0f, 0.0f, 0.0f,
         1.0f, -1.0f,  1.0f,  1.0f, 1.0f, 0.0f, // front
         1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 0.0f,
         1.0f, -1.0f, -1.0f,  1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, -1.0f,  1.0f, 1.0f, 0.0f,
         1.0f, -1.0f, -1.0f,  1.0f, 1.0f, 0.0f,
         1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 0.0f
    };

public:
    glm::f32vec3 position;
	glm::vec3 faceNormals[6];

	void draw_geometry(unsigned int& shaderPgm, unsigned int& vao, unsigned int& vbo);
	Geometry(float br, glm::f32vec3 pos) : br(br), position(pos) {};
	void add_forces(std::vector<Boid>& bd, float coef);
	void calculate_facenormals();
    void draw_lines(unsigned int& shaderPgm, unsigned int& vao, unsigned int& vbo, std::vector<Boid>& bd);
    void avoid_obstacles(std::vector<Boid>& bd, float tc);
    void repel(std::vector<Boid>& bd);
    void check_collision(std::vector<Boid>& bd);
};

