#pragma once
#ifndef VIRTUALCAMERACHANNEL_H
#define VIRTUALCAMERACHANNEL_H

#include "Channel.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <iostream>

class VirtualCameraChannel : public Channel {
public:
    struct KeyFrame {
        float timestamp;
        glm::vec3 position;
        glm::quat rotation;
        glm::vec3 scale;
    };

    VirtualCameraChannel(const std::string& name) : Channel(name, ChannelType::VIRTUAL_CAMERA) {}

    void addKeyFrame(float time, const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale);
    KeyFrame getKeyFrame(float time) const;

    glm::vec3 interpolatePosition(float time) const;
    glm::quat interpolateOrientation(float time) const;
    glm::vec3 interpolateScale(float time) const;

    void update(float deltaTime) override;
    void render(const glm::mat4& view, const glm::mat4& projection) override;

private:
    std::vector<KeyFrame> keyFrames;
};

#endif // VIRTUALCAMERACHANNEL_H
