#include "Definitions.h"

float theta = 0.0f;
float phi = 0.0f;
float camradius = 5.0f;
float cameraspeed = 0.02f;
float camX = camradius;
float camY = 0.0f;
float camZ = 0.0f;

float br = 0.1f; // ball radius
float h = 0.000015f;
float framerate = 30;
float friction_coef = 0.0f;
float coeff_restitution = 1.0f;
float wind_coeff = 0.67f;
float ang_vel = 1.5f;
glm::vec3 rotation_axis = glm::vec3(2.0f, 0.0f,0.0f);
int total_objects = 10;


std::vector<ball_struct> all_objects;
std::vector<std::vector<bool>> collision;

int main(int argc, char ** argv)
{
	srand(time(NULL));

	init_objetcs(total_objects);

	env_factors environment;
	environment.coeff_friction = friction_coef;
	environment.coeff_restitution = coeff_restitution;
	environment.d = wind_coeff;
	environment.g = 9.81f;
	environment.wind_velocity = glm::vec3(glm::linearRand(-5.0f, 1.0f), glm::linearRand(-1.0f, 5.0f), glm::linearRand(-1.0f, 5.0f));

	unsigned int shaderPgm, ballbuffer, boxbuffer, VAO;
	GLFWwindow* window = init_openGL(shaderPgm, ballbuffer, boxbuffer, VAO);
	
	glm::mat4 model = glm::mat4(1.0f);
	unsigned int modelLoc = glGetUniformLocation(shaderPgm, "model");
	glm::mat4 view = glm::mat4(1.0f);
	unsigned int viewLoc = glGetUniformLocation(shaderPgm, "view");
	glm::mat4 projection = glm::mat4(1.0f);
	unsigned int projectionLoc = glGetUniformLocation(shaderPgm, "projection");
	projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	calculate_nomarls();
	time_t cur_time1 = clock();
	time_t cur_time2 = clock();

	while (!glfwWindowShouldClose(window))
	{
		if ((clock() - cur_time1) >(1.0f/framerate))
		{
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUseProgram(shaderPgm);

			view = glm::lookAt(glm::vec3(camX, camY, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			
			glBindVertexArray(VAO);
			draw_domain(shaderPgm, boxbuffer);
			draw_balls(shaderPgm, ballbuffer);

			processInput(window);
			glfwPollEvents();
			glfwSwapBuffers(window);
			cur_time1 = clock();
		}
			rotate_box(ang_vel, rotation_axis);
			calculate_nomarls();
			euler_integration(h, environment);
			cur_time2 = clock();
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &boxbuffer);
	glDeleteBuffers(1, &ballbuffer);
	glDeleteProgram(shaderPgm);
	glfwTerminate();
	return 0;
}