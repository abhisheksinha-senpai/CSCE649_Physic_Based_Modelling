#include "Utility.hpp"

std::vector<float> gaussian_dist;
std::vector<float> uniform_dist;

float theta = 0.0f;
float phi = 0.0f;
float camradius = 7.0f;
float cameraspeed = 0.02f;
float camX = camradius;
float camY = 0.0f;
float camZ = 0.0f;

glm::vec3 cameraPos = glm::vec3(camX, camY, camZ);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

GLFWwindow* init_openGL(unsigned int &shaderPgm, unsigned int& vao, unsigned int& vbo)
{
	try
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		GLFWwindow* window = glfwCreateWindow(800, 600, "CSCE649", NULL, NULL);
		if (window == NULL)
		{
			glfwTerminate();
			throw std::runtime_error("Failed to create GLFW window");
		}
		glfwMakeContextCurrent(window);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			glfwTerminate();
			throw std::runtime_error("Failed to create GLFW window");
		}

		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		glfwSetScrollCallback(window, scroll_callback);
		glfwSetCursorPosCallback(window, mouse_callback);
		glViewport(0, 0, 800, 600);
		glEnable(GL_DEPTH_TEST);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );

		create_shader_program(shaderPgm);
		glUseProgram(shaderPgm);
		float ambient[3] = { 1.0f, 1.0f, 1.0f };
		glUniform4fv(glGetUniformLocation(shaderPgm, "lightColor"), 1, ambient);
		float lightPos[3] = { 3.0f, 3.0f, 3.0f };
		glUniform4fv(glGetUniformLocation(shaderPgm, "lightPos"), 1, lightPos);
		return window;
	}
	catch (std::exception e)
	{
		std::cout << "Unable to initialize openGL and load corresponding modules\n";
		throw;
	}
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}