#include "Definitions.h"
#include "Domain.h"

float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

void calculate_nomarls()
{
    for (int i = 6; i < 216; i += 36)
    {
        glm::vec3 vec1 = glm::vec3(box[i - 6] - box[i], box[i - 5] - box[i + 1], box[i - 4] - box[i + 2]);
        glm::vec3 vec2 = glm::vec3(box[i + 6] - box[i], box[i + 7] - box[i + 1], box[i + 8] - box[i + 2]);
        faceNormals[(int)(i / 36)] = glm::normalize(glm::cross(vec2, vec1));
    }
}

void rotate_box(float angular_speed, glm::vec3 axis)
{
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    glm::mat4 rotationMat(1);
    rotationMat = glm::rotate(rotationMat, angular_speed * h, axis);
    //std::cout << deltaTime << std::endl;
    float new_pos[216] = { 0.0f };
    for (int i = 0; i < 216; i+=6)
    {
        glm::vec4 pos = rotationMat * glm::vec4(box[i], box[i + 1], box[i + 2], 1.0f);
        new_pos[i] = pos[0];
        new_pos[i+1] = pos[1];
        new_pos[i+2] = pos[2];
        new_pos[i+3] = box[i + 3];
        new_pos[i+4] = box[i + 4];
        new_pos[i+5] = box[i + 5];
    }

    memcpy(box, new_pos, 216 * sizeof(float));
}

void draw_domain(unsigned int& shaderPgm, unsigned int& boxbuffer)
{
    // render the box
    glBindBuffer(GL_ARRAY_BUFFER, boxbuffer);
    glBufferData(GL_ARRAY_BUFFER, 216 * sizeof(float), box, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // draw the box (no model transform needed)
    unsigned int modelLoc = glGetUniformLocation(shaderPgm, "model");
    glm::mat4 model = glm::mat4(1.0f);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 36);
}