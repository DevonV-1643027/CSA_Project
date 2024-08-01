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
    void printKeyframesWithInterpolations();
    std::vector<KeyFrame> interpolateKeyFrames() const;

private:
    glm::vec3 interpolatePosition(float time) const;
    glm::quat interpolateOrientation(float time) const;
    glm::vec3 interpolateScale(float time) const;

    void initPathRendering(); // Initialization function

    float currentTime = 0.0f;
    bool traversedPath = false;
    void traversePath(std::vector<KeyFrame> interpolatedKeyFrames);

    GLuint pathVAO, pathVBO;
    GLuint keyframeVAO, keyframeVBO;
    bool isInitialized = false;
    Shader* pathShader, * keyframeShader;
};

#endif // VIRTUALCAMERACHANNEL_H
