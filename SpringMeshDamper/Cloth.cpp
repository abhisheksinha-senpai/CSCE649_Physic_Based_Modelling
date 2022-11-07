#include "Cloth.hpp"

Cloth::Cloth(glm::vec3 pos, glm::vec3 col, int nodes_per_dim, float length, float M)
{
	int N = nodes_per_dim;
	vertices.resize((long)N * (long)N);
	K1_pos.resize((long)N * (long)N);
	K2_pos.resize((long)N * (long)N);
	K3_pos.resize((long)N * (long)N);
	K4_pos.resize((long)N * (long)N);

	K1_vel.resize((long)N * (long)N);
	K2_vel.resize((long)N * (long)N);
	K3_vel.resize((long)N * (long)N);
	K4_vel.resize((long)N * (long)N);

	float delX = (float)length / (float)N;
	int id1, id2;
	center_position = pos;
	glm::vec3 forces = glm::vec3(0);
	glm::vec3 vel = glm::vec3(0);
	mass = M;
	float m = M/ ((float)N * N);
	int blockId, prev_bid, nxt_bid;
	Strut s1;
	Face f1;
	Vertex v;
	int face_cnt = 0;
	for (long i = 0; i < N; i++)
	{
		for (long j = 0; j < N; j++)
		{
			vertices[N * i + j].Position = glm::vec3(center_position.x+-0.5f * length + j * delX, center_position.y, center_position.z+ -0.5f * length + i * delX);
			vertices[N * i + j].mass = m;
			vertices[N * i + j].Color = col;
			vertices[N * i + j].Forces = forces;
			vertices[N * i + j].Velocity = vel;
			vertices[N * i + j].Past_Pos = vertices[N * i + j].Position;
			blockId = 2 * ((N - 1) * i + j);
			prev_bid = 2 * ((N - 1) * (i - 1) + j) + 1;
			nxt_bid = 2 * ((N - 1) * (i + 1) + j) + 1;
			if (i < N - 1 && j < N - 1)
			{
				s1.v_ids[0] = i * N + j;
				s1.v_ids[1] = i * N + j + 1;
				s1.L0 = delX;
				s1.k = tensile_spring_constant;
				s1.d = tensile_spring_damper;
				if (i > 0)
				{
					s1.f_ids[0] = blockId;
					s1.f_ids[1] = prev_bid;
				}
				else
				{
					s1.f_ids[0] = blockId;
					s1.f_ids[1] = -1;
				}
				edge_springs.push_back(s1);

				s1.v_ids[0] = i * N + j;
				s1.v_ids[1] = (i+1) * N + j;
				s1.L0 = delX;
				s1.k = tensile_spring_constant;
				s1.d = tensile_spring_damper;
				if (j > 0)
				{
					s1.f_ids[0] = blockId;
					s1.f_ids[1] = blockId-1;
				}
				else
				{
					s1.f_ids[0] = blockId;
					s1.f_ids[1] = -1;
				}
				edge_springs.push_back(s1);

				s1.v_ids[0] = i * N + j;
				s1.v_ids[1] = (i+1)* N + j + 1;
				s1.L0 = delX*sqrt(2);
				s1.k = shear_spring_constant;
				s1.d = shear_spring_damper;
				s1.f_ids[0] = -1;
				s1.f_ids[1] = -1;
				edge_springs.push_back(s1);

				s1.v_ids[0] = i * N + j +1;
				s1.v_ids[1] = (i + 1) * N + j;
				s1.L0 = delX*sqrt(2);
				s1.k = shear_spring_constant;
				s1.d = shear_spring_damper;
				s1.f_ids[0] = blockId*2;
				s1.f_ids[1] = blockId * 2 + 1;
				edge_springs.push_back(s1);

				f1.ids[0] = i * N + j;
				f1.ids[1] = i* N + j + 1;
				f1.ids[2] = (i + 1) * N + j;
				f1.angles[0] = M_PI_2;
				f1.angles[1] = M_PI_4;
				f1.angles[2] = M_PI_4;
				f1.normal = glm::normalize(glm::cross(vertices[f1.ids[2]].Position - vertices[f1.ids[0]].Position, vertices[f1.ids[1]].Position - vertices[f1.ids[0]].Position));
				vertices[f1.ids[0]].Normals = f1.normal;
				vertices[f1.ids[1]].Normals = f1.normal;
				vertices[f1.ids[2]].Normals = f1.normal;
				surface.push_back(f1);

				f1.ids[0] = (i + 1) * N + j + 1;
				f1.ids[1] = (i + 1) * N + j;
				f1.ids[2] = i * N + j + 1;
				f1.angles[0] = M_PI_2;
				f1.angles[1] = M_PI_4;
				f1.angles[2] = M_PI_4;
				f1.normal = glm::normalize(glm::cross(vertices[f1.ids[2]].Position - vertices[f1.ids[0]].Position, vertices[f1.ids[1]].Position - vertices[f1.ids[0]].Position));
				vertices[f1.ids[0]].Normals = f1.normal;
				vertices[f1.ids[1]].Normals = f1.normal;
				vertices[f1.ids[2]].Normals = f1.normal;
				if (glm::length(f1.normal) < 0.01f)
					std::cout << i << " " << j << std::endl;
				surface.push_back(f1);

				if (i < N - 2 && j < N - 2)
				{
					s1.v_ids[0] = i * N + j;
					s1.v_ids[1] = i * N + j + 2;
					s1.L0 = 2.0f*delX;
					s1.k = bending_spring_constant;
					s1.d = bending_spring_damper;
					s1.f_ids[0] = -1;
					s1.f_ids[1] = -1;
					edge_springs.push_back(s1);

					s1.v_ids[0] = i * N + j;
					s1.v_ids[1] = (i+2) * N + j;
					s1.L0 = 2.0f * delX;
					s1.k = bending_spring_constant;
					s1.d = bending_spring_damper;
					s1.f_ids[0] = -1;
					s1.f_ids[1] = -1;
					edge_springs.push_back(s1);
				}
			}
			else if (j == N - 1 && i < N - 1)
			{
				s1.v_ids[0] = i * N + j;
				s1.v_ids[1] = (i + 1) * N + j;
				s1.L0 = delX;
				s1.k = tensile_spring_constant;
				s1.d = tensile_spring_damper;
				s1.f_ids[0] = blockId - 1;
				s1.f_ids[1] = -1;
				edge_springs.push_back(s1);
			}
			else if (i == N - 1 && j< N - 1)
			{
				s1.v_ids[0] = i * N + j;
				s1.v_ids[1] = i * N + j + 1;
				s1.L0 = delX;
				s1.k = tensile_spring_constant;
				s1.d = tensile_spring_damper;
				s1.f_ids[0] = blockId + 1;
				s1.f_ids[1] = -1;
				edge_springs.push_back(s1);
			}
		}
	}
	setup_render_pipeline();
}

void Cloth::clear_forces()
{
#pragma omp parallel for
	for(int i=0;i<vertices.size();i++)
	{
		vertices[i].Forces=glm::vec3(0.0f, 0.0f, 0.0f);
	}
}

void Cloth::add_gravity()
{
#pragma omp parallel for
	for(int i = 0; i < vertices.size(); i++)
	{
		vertices[i].Forces.y -= 9.8f * vertices[i].mass;
	}
}

void Cloth::add_wind(glm::vec3 vel)
{
#pragma omp parallel for
	for (int i=0;i<surface.size();i++)
	{
		Vertex v1, v2, v3;
		v1 = vertices[surface[i].ids[0]];
		v2 = vertices[surface[i].ids[1]];
		v3 = vertices[surface[i].ids[2]];

		glm::vec3 AB = v2.Position - v1.Position;
		glm::vec3 AC = v3.Position - v1.Position;
		glm::vec3 BC = v3.Position - v2.Position;

		float ang1 = acos(dot(AB, AC)/(glm::length(AB)* glm::length(AC)));
		float ang2 = acos(dot(AB, BC) / (glm::length(AB) * glm::length(BC)));
		float ang3 = M_PI - (ang1 + ang2);

		glm::vec3 cen_vel = (float)(ang1 / M_PI) * v1.Velocity + (float)(ang2 / M_PI) * v2.Velocity + (float)(ang3 / M_PI) * v3.Velocity;
		glm::vec3 rel_vel = vel - cen_vel;

		glm::vec3 someVec = glm::cross(AB, AC);
		float area = 0.5*glm::length(someVec);
		float apparent = area* glm::dot(glm::normalize(someVec), rel_vel);

		glm::vec3 drag = -coef_drag * apparent * rel_vel;

		glm::vec3 int_vel = glm::normalize(glm::cross(glm::normalize(someVec), rel_vel));
		glm::vec3 lift = -coef_lift * apparent * glm::cross(rel_vel, int_vel);

		vertices[surface[i].ids[0]].Forces += (float)(ang1 / M_PI) * (drag + lift);
		vertices[surface[i].ids[1]].Forces += (float)(ang2 / M_PI) * (drag + lift);
		vertices[surface[i].ids[2]].Forces += (float)(ang3 / M_PI) * (drag + lift);
	}
}

void Cloth::setup_render_pipeline()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);           // for vertex data
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);
	for (int i = 0; i < surface.size(); i++)
	{
		indices.push_back(surface[i].ids[0]);
		indices.push_back(surface[i].ids[1]);
		indices.push_back(surface[i].ids[2]);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);   // for index data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normals));

	glBindVertexArray(0);
	printf("%ld %ld %ld %ld\n", vertices.size(), surface.size(), edge_springs.size(), indices.size());
}

void Cloth::draw(unsigned int& shaderPgm)
{
	glUseProgram(shaderPgm);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);           // for vertex data
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);
	model = glm::mat4(1);
	glUniformMatrix4fv(glGetUniformLocation(shaderPgm, "model"), 1, GL_FALSE, glm::value_ptr(model));
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(2);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glLineWidth(1);
	glBindVertexArray(0);
	model = glm::mat4(1);
}

void Cloth::collision_vertex_face(Vertex &v, glm::vec3 mesh_pos, std::vector<Vertex>& mesh, std::vector<Face>&faces, float rad, float delT)
{
	if (glm::length(v.Position - (mesh_pos))-5*rad > glm::length(v.Velocity)*delT )
		return;
#pragma omp parallel for
	for (int i=0;i<faces.size();i++)
	{
		float d1 = glm::dot(v.Position - mesh[faces[i].ids[0]].Position, faces[i].normal);
		float d2 = glm::dot(v.Past_Pos - mesh[faces[i].ids[0]].Position, faces[i].normal);
		float dist = glm::length(v.Position - mesh_pos) - rad;
		if (d1 * d2 < 0.0f && dist <=dist_tol)
		{
			//std::cout << i << " collision" << glm::to_string(faces[i].normal) << std::endl;
			glm::vec3 n = faces[i].normal;
			glm::vec3 v_old = v.Velocity;
			glm::vec3 v_new_n = -coef_res * glm::dot(v_old, n) * n;
			glm::vec3 v_old_t = v_old - glm::dot(v_old, n) * n;
			glm::vec3 v_new_t = (v_old - glm::dot(v_old, n) * n) - std::min(coef_fric * glm::length(v_old_t), glm::length(v_old_t)) * glm::normalize(v_old_t);
			glm::vec3 v_new = v_new_n + v_new_t;
			float d = glm::dot(v.Position - mesh[faces[i].ids[0]].Position, n);
			if (glm::length(v_new) < 0.05f && d <= 0.05f && glm::length(v.Forces) < 0.05f)
			{
				v_new = glm::vec3(0.0f);
				v.Velocity = v_new;
				v.Position = v.Past_Pos;
			}
			else {
				v.Velocity = v_new;
				v.Position = v.Position - (1 + coef_res) * d * n;
			}
		}
	}
}

void Cloth::collosion_edge_edge(Strut &edge, glm::vec3 mesh_pos, std::vector<Vertex>& mesh, std::vector<Face>& faces, float rad, float delT)
{
	float d1 = glm::length(vertices[edge.v_ids[0]].Position - (mesh_pos)) - 5 * rad;
	float d2 = glm::length(vertices[edge.v_ids[1]].Position - (mesh_pos)) - 5 * rad;
	if (d1 > glm::length(vertices[edge.v_ids[0]].Velocity) * delT && d2 > glm::length(vertices[edge.v_ids[1]].Velocity) * delT)
		return;
#pragma omp parallel for
	for (int i = 0; i < faces.size(); i++)
	{
		for (int k = 0; k < 3; k++)
		{
			glm::vec3 a = vertices[edge.v_ids[1]].Position - vertices[edge.v_ids[0]].Position;
			glm::vec3 b = mesh[faces[i].ids[(k+1)%3]].Position - mesh[faces[i].ids[k]].Position;
			glm::vec3 r = mesh[faces[i].ids[k]].Position - vertices[edge.v_ids[0]].Position;
			glm::vec3 n = glm::normalize(glm::cross(a, b));
			glm::vec3 temp = glm::cross(glm::normalize(b), n);
			float s = glm::dot(r, temp) / glm::dot(a, temp);
			temp = glm::cross(glm::normalize(a), n);
			float t = glm::dot(-r, temp) / glm::dot(b, temp);

			glm::vec3 pa = vertices[edge.v_ids[0]].Position + s * a;
			glm::vec3 qa = mesh[faces[i].ids[k]].Position + t * b;

			if (glm::length(qa - pa) <= dist_tol && s>0.0f && s<1.0f && t>0.0f && t<1.0f)
			{
				float u = s;
				float v = 1.0f - u;

				if (v > 1 || u > 1)
					std::cout << u << "  " << v << std::endl;
				glm::vec3 v_old = u * vertices[edge.v_ids[0]].Velocity + v * vertices[edge.v_ids[1]].Velocity;
				glm::vec3 v_new_n = -coef_res * glm::dot(v_old, n) * n;
				glm::vec3 v_old_t = v_old - glm::dot(v_old, n) * n;
				glm::vec3 v_new_t = (v_old - glm::dot(v_old, n) * n) - std::min(coef_fric*glm::length(v_old_t), glm::length(v_old_t))*glm::normalize(v_old_t);
				glm::vec3 v_new = v_new_n + v_new_t;
				glm::vec3 delV = v_new - v_old;

				vertices[edge.v_ids[1]].Velocity +=( v / (u * u + v * v))* delV;
				vertices[edge.v_ids[0]].Velocity += (u / (u * u + v * v)) * delV;
				/*vertices[edge.v_ids[1]].Position = vertices[edge.v_ids[1]].Past_Pos+ vertices[edge.v_ids[1]].Velocity;
				vertices[edge.v_ids[0]].Position = vertices[edge.v_ids[0]].Past_Pos + vertices[edge.v_ids[0]].Velocity;*/
			}
		}
	}
}

void Cloth::add_internal_forces()
{
#pragma omp parallel for
	for (int i = 0; i < edge_springs.size(); i++)
	{
		glm::vec3 diffX = vertices[edge_springs[i].v_ids[1]].Position - vertices[edge_springs[i].v_ids[0]].Position;
		glm::vec3 diffV = vertices[edge_springs[i].v_ids[1]].Velocity - vertices[edge_springs[i].v_ids[0]].Velocity;
		float delX = (glm::length(diffX) - edge_springs[i].L0);
		float delV = glm::dot(diffV, glm::normalize(diffX));
		glm::vec3 force = (edge_springs[i].k * delX + edge_springs[i].d * delV) * glm::normalize(diffX);
		vertices[edge_springs[i].v_ids[0]].Forces += force;
		vertices[edge_springs[i].v_ids[1]].Forces -= force;

		//std::cout << glm::to_string(force) << std::endl;
	}
}

void Cloth::time_march_Euler(float delT)
{
#pragma omp parallel for
	for (int i = 0; i < vertices.size(); i++)
	{
		glm::vec3 new_vel = vertices[i].Velocity + delT * vertices[i].Forces / vertices[i].mass;
		vertices[i].Velocity = new_vel;
		vertices[i].Past_Pos = vertices[i].Position;
		vertices[i].Position += delT * new_vel;
	}

}

void Cloth::time_march_RK4(float delT, bool isWind, glm::vec3 wind)
{
#pragma omp parallel for
	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i].Past_Pos = vertices[i].Position;
		K1_pos[i] = vertices[i].Velocity;
		K1_vel[i] = vertices[i].Forces / vertices[i].mass;
		
		vertices[i].Velocity = K1_pos[i] + 0.5f*delT * vertices[i].Forces / vertices[i].mass;
		vertices[i].Position = vertices[i].Past_Pos + 0.5f * delT * vertices[i].Velocity;
	}

	clear_forces();
	add_gravity();
	add_internal_forces();
	if (isWind)
		add_wind(wind);

#pragma omp parallel for
	for (int i = 0; i < vertices.size(); i++)
	{
		K1_pos[i] = vertices[i].Velocity;
		K1_vel[i] = vertices[i].Forces / vertices[i].mass;

		vertices[i].Velocity = K1_pos[i] + 0.5f * delT * vertices[i].Forces / vertices[i].mass;
		vertices[i].Position = vertices[i].Past_Pos + 0.5f * delT * vertices[i].Velocity;
	}

	clear_forces();
	add_gravity();
	add_internal_forces();
	if (isWind)
		add_wind(wind);

#pragma omp parallel for
	for (int i = 0; i < vertices.size(); i++)
	{
		K3_pos[i] = vertices[i].Velocity;
		K3_vel[i] = vertices[i].Forces / vertices[i].mass;

		vertices[i].Velocity = K1_pos[i] + delT * vertices[i].Forces / vertices[i].mass;
		vertices[i].Position = vertices[i].Past_Pos + delT * vertices[i].Velocity;
	}

	clear_forces();
	add_gravity();
	add_internal_forces();
	if (isWind)
		add_wind(wind);

#pragma omp parallel for
	for (int i = 0; i < vertices.size(); i++)
	{
		K4_pos[i] = vertices[i].Velocity;
		K4_vel[i] = vertices[i].Forces / vertices[i].mass;

		vertices[i].Velocity = K1_pos[i] + (delT /6.0f)*(K1_vel[i]+ 2.0f*K2_vel[i]+ 2.0f*K3_vel[i]+K4_vel[i]);
		vertices[i].Position = vertices[i].Past_Pos + (delT / 6.0f) * (K1_pos[i] + 2.0f * K2_pos[i] + 2.0f * K3_pos[i] + K4_pos[i]);
	}
}

void Cloth::update_constants(float sizeSheet, int num_nodes)
{
	float delX = (float)sizeSheet / (float)num_nodes;
#pragma omp parallel for
	for (int i = 0; i < edge_springs.size(); i++)
	{
		if (edge_springs[i].L0 == delX)
		{
			edge_springs[i].k = tensile_spring_constant;
			edge_springs[i].d = tensile_spring_damper;
		}
		if (edge_springs[i].L0 == sqrt(2.0)*delX)
		{
			edge_springs[i].k = shear_spring_constant;
			edge_springs[i].d = shear_spring_damper;
		}
		if (edge_springs[i].L0 == 2.0f*delX)
		{
			edge_springs[i].k = bending_spring_constant;
			edge_springs[i].d = bending_spring_damper;
		}
	}
}