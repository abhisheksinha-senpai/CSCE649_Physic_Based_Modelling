#pragma once
#define _USE_MATH_DEFINES
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <fstream>
#include <random>
#include <omp.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <cmath>

extern float theta;
extern float phi;
extern float camradius;
extern float cameraspeed;
extern float camX;
extern float camY;
extern float camZ;

const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

const float dist_tol = 0.01f;

extern glm::vec3 cameraPos;
extern glm::vec3 cameraFront;
extern glm::vec3 cameraUp;

extern glm::mat4 view;
extern glm::mat4 model;
extern glm::mat4 projection;

extern float tensile_spring_constant;
extern float bending_spring_constant;
extern float shear_spring_constant;

extern float tensile_spring_damper;
extern float bending_spring_damper;
extern float shear_spring_damper;

extern float coef_lift;
extern float coef_drag;
extern float coef_res;
extern float coef_fric;