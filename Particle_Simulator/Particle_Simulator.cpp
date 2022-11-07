#include "Helpers.hpp"
#include "ParticleEmitter.h"
#include "PotenialField.h"
#include "Planet.hpp"
#include "Asteroid.hpp"

float coef_res = 1.0f;
float coef_fric = 0.0f;

float attraction_coef1 = 2.0f;
float replusion_coef1 = 1.5f;

float attraction_coef2 = 2.0f;
float replusion_coef2 = 1.5f;

float framerate = 30;
float delH = 0.001f;
int num_obj = 1;

extern std::vector<float> gaussian_dist;
extern std::vector<float> uniform_dist;
float h = 20.0f;

std::vector<float>arrow_data;
int total_particles = 1;
int main()
{
	srand(time(NULL));

	unsigned int shaderPgm, vao, vbo;
	GLFWwindow* window = init_openGL(shaderPgm, vao, vbo);

	time_t cur_time1 = clock();
	time_t cur_time2 = clock();

	glm::mat4 view = glm::mat4(1.0f);
	unsigned int viewLoc = glGetUniformLocation(shaderPgm, "view");
	Planet bb;

	std::vector<Asteroid>asteriod_field;
	float b;
	int ast = 0;
	glm::f32vec3 pos = glm::f32vec3(1.0f, 0.8f, -0.3f);
	b = uniform_dist[ast++] * (0.7 - 0.5f) + 0.5f;
	for (int i = 0; i < num_obj; i++)
	{
		b = uniform_dist[ast++] * (0.7 - 0.5f) + 0.5f;
		pos = glm::f32vec3(1.0f, 0.8f, -0.3f);
		ast = ast % (3 * MAX_PARTICLES);
		Asteroid a = Asteroid(b, pos);
		asteriod_field.push_back(a);
	}

	ParticleEmitter bh1 = ParticleEmitter(glm::f32vec3{ pos.x+0.75f*b, pos.y + 0.65f*b, pos.z + 0.65f*b }, glm::f32vec3{ 0.2f, 0.5f, 0.5f }, 0.5f, 0.2f, attraction_coef1, replusion_coef1);
	PotenialField ph1 = PotenialField(attraction_coef1, replusion_coef1, bh1.inner_radius, bh1.position);

	ParticleEmitter bh2 = ParticleEmitter(glm::f32vec3{ pos.x + 0.15f * b, pos.y + 0.05f * b, pos.z + 0.15f * b }, glm::f32vec3{ 0.2f, 0.5f, 0.5f }, 0.5f, 0.2f, attraction_coef2, replusion_coef2);
	PotenialField ph2 = PotenialField(attraction_coef2, replusion_coef2, bh2.inner_radius, bh2.position);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//bh1.generate_particles(5);

	//bh1.generate_particles(1000);
	float angle = 0.0f;
	while (!glfwWindowShouldClose(window))
	{
		if (((float)clock() - (float)cur_time2)/ CLOCKS_PER_SEC > delH)
		{
			cur_time2 = clock();
			
			angle =angle + delH ;
			bh1.position = glm::f32vec3(glm::sin(angle * glm::pi<float>()), glm::cos(angle * glm::pi<float>()), 0.0f);
			for (int i = 0; i < num_obj; i++)
			{
				asteriod_field[i].position = glm::f32vec3(glm::sin(angle * glm::pi<float>()), bh1.position.y, glm::cos(angle * glm::pi<float>()));
			}
			ph1.position = bh1.position;
			bh1.generate_particles(int(1000.0f * 0.1f));
			ph1.add_force(bh1.particles);
			bh1.EulerIntegration(delH, asteriod_field);
			bh1.update_particles(delH);


			bh2.position = glm::f32vec3(glm::cos(angle * glm::pi<float>()), pos.y, glm::sin(angle * glm::pi<float>()));
			ph2.position = bh2.position;
			bh2.generate_particles(int(1000.0f * 0.1f));
			ph2.add_force(bh2.particles);
			bh2.EulerIntegration(delH, asteriod_field);
			bh2.update_particles(delH);
			
		}
		if (((float)clock() - (float)cur_time1)/ CLOCKS_PER_SEC > (1.0f/framerate))
		{
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUseProgram(shaderPgm);

			view = glm::lookAt(cameraPos, cameraPos+cameraFront, cameraUp);
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			bb.draw(shaderPgm, ph1.position, vao, vbo);
			bh1.draw_particles(shaderPgm, vao, vbo);
			for (int i = 0; i < num_obj; i++)
			{
				asteriod_field[i].draw_asteroid(shaderPgm, vao, vbo);
			}
			bb.draw(shaderPgm, ph2.position, vao, vbo);
			bh2.draw_particles(shaderPgm, vao, vbo);
			
			////draw_stuff(vao, vbo);
			
			processInput(window);
			glfwPollEvents();
			glfwSwapBuffers(window);
			cur_time1 = clock();
			std::cout << "Num particles : " << total_particles << std::endl;
		}
		
	}
	glDeleteProgram(shaderPgm);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	glfwTerminate();
	return 0;
}

void draw_stuff(unsigned int &vao, unsigned int &vbo)
{
	//std::cout << arrow_vec.size() << std::endl;
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, arrow_data.size() * sizeof(float), arrow_data.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
	glEnableVertexAttribArray(1);
	glDrawArrays(GL_LINES, 0, arrow_data.size());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}