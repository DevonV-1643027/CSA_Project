#ifndef VIRTUALCAMERACHANNEL_H
#define VIRTUALCAMERACHANNEL_H

#include <GL/glew.h>
#include "Channel.h"
#include "Shader.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

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

    GLuint pathVAO, pathVBO;
    bool isInitialized = false;
    Shader* pathShader;
};

#endif // VIRTUALCAMERACHANNEL_H
