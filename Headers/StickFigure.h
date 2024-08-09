#pragma once
#ifndef STICKFIGURE_H
#define STICKFIGURE_H

#define GLM_ENABLE_EXPERIMENTAL

// for object loading I use the learnopengl implementation, all credits go to the authors
#include <learnopengl/model.h>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

// Define a Joint structure
struct Joint {
    std::string name; // Name of the joint for clarity
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
    std::vector<int> childrenIndices;
};

class StickFigure {
public:
    StickFigure();
    void render(const glm::mat4& view, const glm::mat4& projection);
    glm::mat4 getJointModelMatrix(const Joint& joint);
    
private:
    GLuint cylinderVAO, cylinderVBO, sphereVAO, sphereVBO;
    GLuint cylinderIndexCount, sphereIndexCount;
    Shader* shader = nullptr;
    void setupSphere();
    void setupCylinder();

    std::vector<Joint> skeleton;
    
    void drawCylinder(const glm::mat4& modelMatrix, const glm::mat4& view, const glm::mat4& projection);
    void drawSphere(const glm::mat4& modelMatrix, const glm::mat4& view, const glm::mat4& projection);

    Joint createJoint(const std::string& name, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale);
    int getParentIndex(int jointIndex) const;
};

#endif // STICKFIGURE_H
