#pragma once
#include "Definitions.hpp"

void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
GLFWwindow* init_openGL(unsigned int& shaderPgm, unsigned int& vao, unsigned int& vbo);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void create_shader_program(unsigned int& shaderProgram);
void load_shader(std::string file_name, std::string& location);
void draw_window(bool* show_demo_window, bool* show_another_window, ImVec4* clear_color);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void draw_rays(unsigned int& shaderPgm, glm::vec3 pos);