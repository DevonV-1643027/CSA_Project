#pragma once
#ifndef VIRTUALCAMERACHANNEL_H
#define VIRTUALCAMERACHANNEL_H

#include "Channel.h"

class VirtualCameraChannel : public Channel {
public:
    VirtualCameraChannel(const std::string& name) : Channel(name, ChannelType::VIRTUAL_CAMERA) {}
    void update(float deltaTime) override {
        // Basic update logic for VirtualCameraChannel
    }
    virtual void render(const glm::mat4& view, const glm::mat4& projection) override {
        // Basic render logic for CharacterAnimationChannel
    }
};

#endif // VIRTUALCAMERACHANNEL_H

