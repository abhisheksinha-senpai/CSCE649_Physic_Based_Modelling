#include "Utility.hpp"
#include "Sphere.hpp"
#include "Cloth.hpp"




float tensile_spring_constant = 10.0f;
float bending_spring_constant = 0.01f;
float shear_spring_constant = 0.1f;

float tensile_spring_damper = 1.0f;
float bending_spring_damper = 0.01f;
float shear_spring_damper = 0.1f;

float coef_lift = 0.4f;
float coef_drag = 0.2f;
float coef_res = 0.051f;
float coef_fric = 0.95f;

float framerate = 30;
float delH = 0.006f;

float windX = 0.1f;
float windY = 0.2f;
float windZ = 2.3f;

glm::mat4 view = glm::mat4(1.0f);
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 projection = glm::mat4(1.0f);
int main()
{
	srand(clock());

	unsigned int shaderPgm, vao, vbo;
	GLFWwindow* window = init_openGL(shaderPgm, vao, vbo);

	time_t cur_time2 = clock();
	time_t cur_time1 = clock();

	printf("%s %s %s\n", glGetString(GL_VERSION), glGetString(GL_VENDOR), glGetString(GL_RENDERER));

	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	const char* glsl_version = "#version 420";
	ImGui_ImplOpenGL3_Init(glsl_version);

	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	Sphere s1 = Sphere(0.5f, glm::vec3(0.75f), glm::vec3(0.75f, 0.95f, 0.45f));
	Sphere s2 = Sphere(0.3f, glm::vec3(1.75f, 0.95, 1.45f), glm::vec3(0.875f, 0.12f, 0.65f));
	float size_of_sheet = 3.0f;
	int num_nodes = 21;
	Cloth c1 = Cloth(glm::vec3(1.2f, 2.0f, 1.2f), glm::vec3(0.0f), num_nodes, size_of_sheet, 5);

	while (!glfwWindowShouldClose(window))
	{
		if (  (((float)clock() - (float)cur_time2) / CLOCKS_PER_SEC)> (1.0f/ framerate) )
		{
			glClearColor(1.05f, 1.05f, 1.01f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUseProgram(shaderPgm);
			float ambient[3] = { 1.0f, 1.0f, 1.0f };
			glUniform3fv(glGetUniformLocation(shaderPgm, "lightColor"), 1, ambient);
			float lightPos[3] = { 3.0f, 3.0f, 3.0f };
			glUniform3fv(glGetUniformLocation(shaderPgm, "lightPos"), 1, lightPos);

			draw_window(&show_demo_window, &show_another_window, &clear_color);
			projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			glUniformMatrix4fv(glGetUniformLocation(shaderPgm, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

			view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
			glUniformMatrix4fv(glGetUniformLocation(shaderPgm, "view"), 1, GL_FALSE, glm::value_ptr(view));

			
			//draw_rays(shaderPgm, glm::vec3(1.2f));
			ImGui_ImplGlfw_NewFrame();
			s1.draw(shaderPgm);
			s2.draw(shaderPgm);
			c1.draw(shaderPgm);
			processInput(window);
			glfwPollEvents();
			glfwSwapBuffers(window);
			cur_time2 = clock();
		}
		if ((((float)clock() - (float)cur_time1) / CLOCKS_PER_SEC) >3.0*delH)
		{
			c1.clear_forces();
			c1.add_gravity();
			c1.update_constants(size_of_sheet, num_nodes);
			c1.add_internal_forces();
			c1.add_wind(glm::vec3(windX, windY, windZ));
			//c1.time_march_Euler(delH);
			c1.time_march_RK4(delH, true, glm::vec3(windX, windY, windZ));
#pragma omp parallel for
			for (int i = 0; i < c1.vertices.size(); i++)
			{
				c1.collision_vertex_face(c1.vertices[i], s1.center_position, s1.vertices, s1.surface, s1.radius, delH);
				c1.collision_vertex_face(c1.vertices[i], s2.center_position, s2.vertices, s2.surface, s2.radius, delH);
			}
#pragma omp parallel for
			for (int i = 0; i < c1.edge_springs.size(); i++)
			{
				c1.collosion_edge_edge(c1.edge_springs[i], s1.center_position, s1.vertices, s1.surface, s1.radius, delH);
				c1.collosion_edge_edge(c1.edge_springs[i], s2.center_position, s2.vertices, s2.surface, s2.radius, delH);
			}
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

void draw_window(bool* show_demo_window, bool* show_another_window, ImVec4* clear_color)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	{
		ImGui::Begin("CSCE649");                          // Create a window called "Hello, world!" and append into it.

		ImGui::SliderFloat("Tensile Sp con", &tensile_spring_constant, 50.0f, 500.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::SliderFloat("Bending Sp con", &bending_spring_constant, 0.001f, 1.0f);
		ImGui::SliderFloat("Shear sp con", &shear_spring_constant, 1.0f, 50.0f);

		ImGui::SliderFloat("Tensile Damp con", &tensile_spring_damper, 5.0f, 50.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::SliderFloat("Bending Damp con", &bending_spring_damper, 0.001f, 0.1f);
		ImGui::SliderFloat("Shear Damp con", &shear_spring_damper, 0.1f, 5.0f);

		ImGui::SliderFloat("Wind X", &windX, 0.0f, 5.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::SliderFloat("Wind Y", &windY, 0.0f, 5.0f);
		ImGui::SliderFloat("Wind Z ", &windZ, 0.0f, 5.0f);

		ImGui::ColorEdit3("clear color", (float*)clear_color); // Edit 3 floats representing a color
		if (ImGui::Button("DelH+"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			delH += 0.0005;
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

void draw_rays(unsigned int &shaderPgm, glm::vec3 pos)
{
	glUseProgram(shaderPgm);
	unsigned int VAO, VBO;
	float ray_data[12] = { 0,0,0, 0.5f, 0.175f, 0.75f, pos.x, pos.y, pos.z, 0.5f, 0.175f, 0.75f };
	glUseProgram(shaderPgm);
	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);           // for vertex data
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), ray_data, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
	model = glm::mat4(1);
	glUniformMatrix4fv(glGetUniformLocation(shaderPgm, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glLineWidth(12);
	glDrawArrays(GL_LINES, 0, 2);
	glLineWidth(1);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);

}