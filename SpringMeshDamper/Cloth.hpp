#pragma once
#include "Node.hpp"

class Cloth
{
	std::vector<Face> surface;
	std::vector<int> indices;
	glm::vec3 center_position;
	glm::vec3 center_velocity = glm::vec3(0.0f);

	std::vector < glm::vec3> K1_pos;
	std::vector < glm::vec3> K2_pos;
	std::vector < glm::vec3> K3_pos;
	std::vector < glm::vec3> K4_pos;

	std::vector < glm::vec3> K1_vel;
	std::vector < glm::vec3> K2_vel;
	std::vector < glm::vec3> K3_vel;
	std::vector < glm::vec3> K4_vel;
	
	float mass;
	unsigned int VAO, VBO, EBO;
public:
	Cloth(glm::vec3 pos, glm::vec3 col, int nodes_per_dim, float length, float M);
	std::vector<Vertex> vertices;
	std::vector<Strut> edge_springs;
	void add_gravity();
	void add_wind(glm::vec3 vel);
	void add_internal_forces();
	void collision_vertex_face(Vertex& v, glm::vec3 mesh_pos, std::vector<Vertex>& mesh, std::vector<Face>& faces, float rad, float delT);
	void collosion_edge_edge(Strut& edge, glm::vec3 mesh_pos, std::vector<Vertex>& mesh, std::vector<Face>& faces, float rad, float delT);
	void time_march_Euler(float delT);
	void clear_forces();
	void setup_render_pipeline();
	void time_march_RK4(float delT, bool isWind, glm::vec3 wind);
	void draw(unsigned int &shaderPgm);
	void update_constants(float sizeSheet, int num_nodes);
};

