#include "Definitions.hpp"
#include "Helpers.hpp"
#include "Boid.hpp"
#include "Geometry.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <omp.h>

float framerate = 30;
float delH = 0.001f;

extern std::vector<float> gaussian_dist;
extern std::vector<float> uniform_dist;


float coef_avoid=0.01f;
float coef_vel_match = 0.1f;
float coef_center = 0.01f;

float frontal_vision=glm::pi<float>()/4;
float peripheral_vision = 2*glm::pi<float>() / 3;

float inner_rad = 0.2f;
float outer_rad = 0.5f;

float banking_multiplier = 0.1;
float force_coef = 0.25f;
int num_objects = 100;
float radius = 5.0f;
void draw_window(bool* show_demo_window, bool* show_another_window, ImVec4* clear_color);

int main()
{
	srand(clock());

	unsigned int shaderPgm, vao, vbo;
	GLFWwindow* window = init_openGL(shaderPgm, vao, vbo);

	time_t cur_time1 = clock();
	time_t cur_time2 = clock();

	glm::mat4 view = glm::mat4(1.0f);
	unsigned int viewLoc = glGetUniformLocation(shaderPgm, "view");

	Geometry attractor = Geometry(0.75f, glm::f32vec3(0.5f, 0.75f, -0.73f));

	Geometry outer_boundary = Geometry(radius, glm::f32vec3(0.0f, 0.0f, 0.0f));
	outer_boundary.calculate_facenormals();

	std::vector< Geometry> obstacles;
	for (int i = 0; i < num_objects; i++)
	{
		obstacles.push_back(Geometry(0.1, glm::f32vec3(glm::linearRand(-radius, radius), glm::linearRand(-radius, radius), glm::linearRand(-radius, radius))));
	}
	printf("%s %s %s\n", glGetString(GL_VERSION), glGetString(GL_VENDOR), glGetString(GL_RENDERER));
	FlockingSystem birds1(glm::f32vec3(-1.0f, 0.0f, -1.0f), glm::f32vec3(0.1f, 0.0f, 0.0f));
	birds1.generate_particles(100);

	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	const char* glsl_version = "#version 420";
	ImGui_ImplOpenGL3_Init(glsl_version);

	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	float angle = 0.0f;
	while (!glfwWindowShouldClose(window))
	{
		if (((float)clock() - (float)cur_time2)/ CLOCKS_PER_SEC > delH)
		{
			angle = angle + delH;
			attractor.position = glm::f32vec3(4*glm::sin(angle * glm::pi<float>()), 4 * glm::cos(angle * glm::pi<float>())* glm::sin(angle * glm::pi<float>()), 2*glm::cos(angle * glm::pi<float>()));
			//attractor.position = glm::f32vec3(2 * glm::sin(angle * glm::pi<float>()), 0.0f, 2 * glm::cos(angle * glm::pi<float>()));
			cur_time2 = clock();
			attractor.add_forces(birds1.BoidSystem, 1.25f);
			birds1.add_forces();
			for (int i = 0; i < num_objects; i++)
				obstacles[i].avoid_obstacles(birds1.BoidSystem, 100*delH);
			outer_boundary.repel(birds1.BoidSystem);
			birds1.calculate_new_state_vector(delH);
			
			
		/*}
		if (((float)clock() - (float)cur_time1)/ CLOCKS_PER_SEC > (1.0f/framerate))
		{*/
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUseProgram(shaderPgm);

			view = glm::lookAt(cameraPos, cameraPos+cameraFront, cameraUp);
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			ImGui_ImplGlfw_NewFrame();
			//attractor.draw_geometry(shaderPgm, vao, vbo);
			birds1.draw_boids(shaderPgm, vao, vbo);
			for (int i = 0; i < num_objects; i++)
			{
				obstacles[i].check_collision(birds1.BoidSystem);
				obstacles[i].draw_geometry(shaderPgm, vao, vbo);
			}
			processInput(window);
			glfwPollEvents();
			draw_window(&show_demo_window, &show_another_window, &clear_color);

			glfwSwapBuffers(window);
			cur_time1 = clock();
		}
		
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glDeleteProgram(shaderPgm);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	glfwTerminate();
	return 0;
}

void draw_window(bool * show_demo_window, bool * show_another_window, ImVec4 * clear_color)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	{
		ImGui::Begin("CSCE649");                          // Create a window called "Hello, world!" and append into it.

		ImGui::SliderFloat("Coef Avoidance", &coef_avoid, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::SliderFloat("Coef Vel matching", &coef_vel_match, 0.0f, 1.0f);
		ImGui::SliderFloat("Coef Centering", &coef_center, 0.0f, 1.0f);
		ImGui::SliderFloat("Attractive force coef", &force_coef, 0.0f, 1.0f);
		ImGui::ColorEdit3("clear color", (float*)clear_color); // Edit 3 floats representing a color
		if (ImGui::Button("DelH+"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			delH +=0.0005;
		ImGui::SameLine();
		if (ImGui::Button("DelH-"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			delH -= 0.0005;
		if (delH < 0.001f)
			delH = 0.001f;
		ImGui::Text("counter = %f", delH);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}