#pragma once
#ifndef BACKGROUNDCHANNEL_H
#define BACKGROUNDCHANNEL_H

#include "../Headers/Channel.h"

class BackgroundChannel : public Channel {
public:
    BackgroundChannel(const std::string& name) : Channel(name) {}
    void update(float deltaTime) override {
        // Basic update logic (if any)
    }
    void render() override {
        // Basic render logic (if any)
    }
};

#endif // BACKGROUNDCHANNEL_H

