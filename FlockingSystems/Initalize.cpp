#include "Definitions.hpp"

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

		glm::mat4 model = glm::mat4(1.0f);
		unsigned int modelLoc = glGetUniformLocation(shaderPgm, "model");
		glm::mat4 view = glm::mat4(1.0f);
		unsigned int viewLoc = glGetUniformLocation(shaderPgm, "view");
		glm::mat4 projection = glm::mat4(1.0f);
		unsigned int projectionLoc = glGetUniformLocation(shaderPgm, "projection");
		projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		std::default_random_engine generator_;
		static std::normal_distribution<float> norm_dist(0.0f, 1.0f);
		static std::uniform_real_distribution<float> uni_float_dist(0.0f, 1.0f);
		for (int i = 0; i < 5 * MAX_PARTICLES; i++)
		{
			gaussian_dist.push_back(norm_dist(generator_));
			uniform_dist.push_back(uni_float_dist(generator_));
		}
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
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