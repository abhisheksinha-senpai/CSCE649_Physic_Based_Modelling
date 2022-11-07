#pragma once
#include "Definitions.hpp"
#include "Node.hpp"

class Sphere
{
private:
    
    std::vector<int> indices;
    glm::vec3 color;
    unsigned int VAO, VBO, EBO, VBO_1, VAO_1;
public:
    float radius;
    std::vector<Vertex>vertices;
    std::vector<Face> surface;
    glm::vec3 center_position;
    Sphere(float rad, glm::vec3 pos, glm::vec3 col);
    void setup_render_pipeline();
    void draw(unsigned int& shaderPgm);
    ~Sphere();
};

