#include "Sphere.hpp"

Sphere::Sphere(float rad, glm::vec3 pos, glm::vec3 col):radius(rad), center_position(pos), color(col)
{
    int sectorCount = 12;
    int stackCount = 9;
    float x, y, z, xy; 
    float nx, ny, nz, lengthInv = 1.0f / radius;
    float s, t; 

    float sectorStep = 2 * M_PI / sectorCount;
    float stackStep = M_PI / stackCount;
    float sectorAngle, stackAngle;

    for (int i = 0; i <= stackCount; ++i)
    {
        stackAngle = M_PI_2 - (float)(i) * stackStep;
        xy = radius * cosf(stackAngle);
        z = radius * sinf(stackAngle);
        for (int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;

            x = xy * cosf(sectorAngle);
            y = xy * sinf(sectorAngle); 
            glm::vec3 POS = center_position+glm::vec3(x, y, z);
            glm::vec3 Color = glm::vec3(color.x, color.y, color.z);
            Vertex v = { POS, glm::vec3(0.0f), POS, glm::vec3(0), glm::vec3(0), Color, 99999.0f};
            vertices.push_back(v);
        }
    }
    int k1, k2;
    Face f1;
    for (int i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);     // beginning of current stack
        k2 = k1 + sectorCount + 1;      // beginning of next stack

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            if (i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);

                f1.ids[0] = k1;
                f1.ids[1] = k2;
                f1.ids[2] = k1 + 1;
                f1.angles[0] = 0;
                f1.angles[1] = 0;
                f1.angles[2] = 0;
                f1.normal = glm::normalize(glm::cross(vertices[k2].Position - vertices[k1].Position, vertices[k1 + 1].Position - vertices[k1].Position));
                vertices[k1].Normals += f1.normal/3.0f;
                vertices[k2].Normals += f1.normal / 3.0f;
                vertices[k1 + 1].Normals += f1.normal / 3.0f;
                surface.push_back(f1);
            }

            // k1+1 => k2 => k2+1
            if (i != (stackCount - 1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);

                f1.ids[0] = k1 + 1;
                f1.ids[1] = k2;
                f1.ids[2] = k2 + 1;
                f1.angles[0] = 0;
                f1.angles[1] = 0;
                f1.angles[2] = 0;
                f1.normal = glm::normalize(glm::cross(vertices[k1+1].Position - vertices[k2].Position, vertices[k2 + 1].Position - vertices[k2].Position));
                vertices[k1+1].Normals += f1.normal / 3.0f;
                vertices[k2].Normals += f1.normal / 3.0f;
                vertices[k2 + 1].Normals += f1.normal / 3.0f;
                surface.push_back(f1);
            }
        }
    }
    setup_render_pipeline();
}

void Sphere::setup_render_pipeline()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);           // for vertex data
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);   // for index data
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(int), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normals));
    glBindVertexArray(0);

    glGenVertexArrays(1, &VAO_1);
    glGenBuffers(1, &VBO_1);
    glBindVertexArray(VAO_1);
    std::vector<float> temp;
    for (int i = 0; i < surface.size(); i++)
    {
        for (int k = 0; k < 3; k++)
        {
            temp.push_back(vertices[surface[i].ids[k]].Position.x);
            temp.push_back(vertices[surface[i].ids[k]].Position.y);
            temp.push_back(vertices[surface[i].ids[k]].Position.z);

            temp.push_back(0.55);
            temp.push_back(0.65);
            temp.push_back(0.34);

            temp.push_back(vertices[surface[i].ids[k]].Position.x + 0.1*surface[i].normal.x);
            temp.push_back(vertices[surface[i].ids[k]].Position.y + 0.1 * surface[i].normal.y);
            temp.push_back(vertices[surface[i].ids[k]].Position.z + 0.1 * surface[i].normal.z);

            temp.push_back(0.55);
            temp.push_back(0.65);
            temp.push_back(0.34);
        }
    }
    std::cout << "size of normals " << temp.size() / 18 << std::endl;
    std::cout << "size of surface " << surface.size() << std::endl;
    std::cout << "size of indices " << indices.size() << std::endl;
    glBindBuffer(GL_ARRAY_BUFFER, VBO_1);           // for vertex data
    glBufferData(GL_ARRAY_BUFFER, temp.size() * sizeof(float), temp.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)12);
    glBindVertexArray(0);

}

void Sphere::draw(unsigned int& shaderPgm)
{
    glUseProgram(shaderPgm);
    glBindVertexArray(VAO);
    model = glm::mat4(1);
    glUniformMatrix4fv(glGetUniformLocation(shaderPgm, "model"), 1, GL_FALSE, glm::value_ptr(model));
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(2);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glLineWidth(1);
    glBindVertexArray(0);

    //glBindVertexArray(VAO_1);
    //glLineWidth(5);
    //glDrawArrays(GL_LINES, 0, surface.size() * 18);
    //glLineWidth(1);
    //glBindVertexArray(0);
}

Sphere::~Sphere()
{
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);

    glDeleteBuffers(1, &VBO_1);
    glDeleteVertexArrays(1, &VAO_1);
}