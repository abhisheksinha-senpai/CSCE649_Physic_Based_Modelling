#include "Definitions.h"

GLFWwindow* init_openGL(unsigned int &shaderPgm, unsigned int &ballbuffer, unsigned int &boxbuffer, unsigned int &VAO)
{
	try
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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
		glViewport(0, 0, 800, 600);

		create_shader_program(shaderPgm);
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glGenBuffers(1, &boxbuffer);
		glGenBuffers(1, &ballbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, boxbuffer);
		glBufferData(GL_ARRAY_BUFFER, 216 * sizeof(float), box, GL_STATIC_DRAW);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

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