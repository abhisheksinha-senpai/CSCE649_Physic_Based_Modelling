#pragma once

#include <iostream>
#include <sstream>
#include <string.h>
#include <fstream>
#include <vector>
#include <algorithm>
#include <time.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//#define M_PI 3.14159265358979323846

// View parameters
extern float theta;
extern float phi;
extern float camradius;
extern float cameraspeed;
extern float camX;
extern float camY;
extern float camZ;

extern float radius;
extern int sectorCount;
extern int stackCount;

extern std::vector<float> vertices;
extern std::vector<int> indices;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const float distance_tolerance = 0.01f;
const float velocity_tolerance = 0.01f;
const float accel_tolerance = 0.01f;

extern float box[];
extern float br;
extern float ball[];
extern glm::vec3 faceNormals[];
extern float deltaTime;
extern float h;
extern float framerate;

struct ball_struct
{
	float m;
	glm::vec3 position;
	glm::vec3 velocity;
};

extern std::vector<ball_struct> all_objects;
extern std::vector<std::vector<bool>> collision;

struct env_factors
{
	float g;
	float d;
	glm::vec3 wind_velocity;
	float coeff_restitution;
	float coeff_friction;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
GLFWwindow* init_openGL(unsigned int& shaderPgm, unsigned int& ballbuffer, unsigned int& boxbuffer, unsigned int& VAO);
void load_shader(std::string file_name, std::string& location);
void create_shader_program(unsigned int& shaderProgram);
void create_ball();
int detect_collision_plane(glm::vec3 new_position, glm::vec3 old_position, int id);
bool detect_collision_ball(int myID, int ball_ID);
void calculate_nomarls();
float calculate_fraction(glm::vec3& new_position, ball_struct& ball, int id);
glm::vec3 calculate_accel(ball_struct& ball, env_factors& factors);
glm::vec3 collision_response_plane(glm::vec3 &new_velocity, int id, float coeff_restitution, float friction);
void collision_response_ball(int myID, int other_id, float coeff_restitution);
void euler_integration(float timestep, env_factors& factors);
void rotate_box(float angular_speed, glm::vec3 axis);
void checkInsideDomain(ball_struct& ball);
void init_objetcs(int n);
void draw_balls(unsigned int& shaderPgm, unsigned int& ballbuffer);
void draw_domain(unsigned int& shaderPgm, unsigned int& boxbuffer);