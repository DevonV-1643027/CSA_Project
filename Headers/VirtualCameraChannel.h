#ifndef VIRTUALCAMERACHANNEL_H
#define VIRTUALCAMERACHANNEL_H

#include <GL/glew.h>
#include "Channel.h"
#include "Shader.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"
#include <thread>
#include <chrono>

class VirtualCameraChannel : public Channel {
public:
    VirtualCameraChannel(const std::string& name);
    void update(float deltaTime) override;
    void render(const glm::mat4& view, const glm::mat4& projection) override;
    void printKeyframesWithInterpolations(std::vector<KeyFrame> interpolatedKeyFrames);
    std::vector<KeyFrame> interpolateKeyFrames() const;
    void startTraversal(); // Method to start traversal

private:
    glm::vec3 interpolatePosition(float time) const;
    glm::quat interpolateOrientation(float time) const;
    glm::vec3 interpolateScale(float time) const;

    void initPathRendering(); // Initialization function

    float currentTime = 0.0f;
    bool traversalComplete = false; // Flag to indicate traversal is complete
    bool isTraversalInProgress = false; // Flag to check if traversal is in progress

    std::vector<KeyFrame> interpolatedKeyFrames; // Store interpolated keyframes for traversal
    void traversePath(); // Simplified traversePath method

    GLuint pathVAO, pathVBO;
    GLuint keyframeVAO, keyframeVBO;
    bool isInitialized = false;
    Shader* pathShader, * keyframeShader;

    glm::vec3 bezierInterpolate(const std::vector<glm::vec3>& points, float t) const;
    glm::quat bezierInterpolate(const std::vector<glm::quat>& points, float t) const;
};

#endif // VIRTUALCAMERACHANNEL_H
