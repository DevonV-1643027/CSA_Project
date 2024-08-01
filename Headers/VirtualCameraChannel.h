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
    VirtualCameraChannel(const std::string& name) : Channel(name, ChannelType::VIRTUAL_CAMERA) {}

    glm::vec3 interpolatePosition(float time) const;
    glm::quat interpolateOrientation(float time) const;
    glm::vec3 interpolateScale(float time) const;

    void update(float deltaTime) override;
    void render(const glm::mat4& view, const glm::mat4& projection) override;
    
    void printKeyframesWithInterpolations();
    std::vector<KeyFrame> interpolateKeyFrames() const;
};

#endif // VIRTUALCAMERACHANNEL_H
