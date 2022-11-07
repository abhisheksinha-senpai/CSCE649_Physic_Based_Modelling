#pragma once
#include <iostream>
#include <string>
#include <algorithm>
#include <time.h>
#include <vector>
#include <random>
#include <queue>
#include <sstream>
#include <fstream>


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <unordered_map>

#define MAX_PARTICLES 2000
#define PARTICLE_LIFE 1

extern float theta;
extern float phi;
extern float camradius;
extern float cameraspeed;
extern float camX;
extern float camY;
extern float camZ;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const float dist_tol = 0.01f;

extern float coef_res;
extern float coef_fric;

extern glm::vec3 cameraPos;
extern glm::vec3 cameraFront;
extern glm::vec3 cameraUp;
extern int total_particles;

GLFWwindow* init_openGL(unsigned int& shaderPgm, unsigned int& vao, unsigned int& vbo);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void load_shader(std::string file_name, std::string& location);
void create_shader_program(unsigned int& shaderProgram);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void draw_stuff(unsigned int& vao, unsigned int& vbo);
