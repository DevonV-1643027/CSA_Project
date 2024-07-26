#pragma once
#ifndef VIRTUALCAMERACHANNEL_H
#define VIRTUALCAMERACHANNEL_H

#include "../Headers/Channel.h"

class VirtualCameraChannel : public Channel {
public:
    VirtualCameraChannel(const std::string& name) : Channel(name, ChannelType::VIRTUAL_CAMERA) {}
    void update(float deltaTime) override {
        // Basic update logic for VirtualCameraChannel
    }
    void render() override {
        // Basic render logic for VirtualCameraChannel
    }
};

#endif // VIRTUALCAMERACHANNEL_H

