#pragma once
#include "Definitions.hpp"

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normals;
	glm::vec3 Past_Pos;
	glm::vec3 Velocity;
	glm::vec3 Forces;
	glm::vec3 Color;
	float mass;
};

struct Strut
{
	int v_ids[2];
	int f_ids[2] = { -1, -1 };
	float L0;
	float k;
	float d;
};

struct Face
{
	float angles[3];
	glm::vec3 normal;
	int ids[3];
};
