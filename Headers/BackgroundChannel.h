#pragma once
#ifndef BACKGROUNDCHANNEL_H
#define BACKGROUNDCHANNEL_H

#include "../Headers/Channel.h"
#include <string>
#include <glm/glm.hpp>

class BackgroundChannel : public Channel {
public:
    BackgroundChannel(const std::string& name);
    virtual ~BackgroundChannel() {}

    void loadTexture(const std::string& filePath);

    virtual void update(float deltaTime) override;
    virtual void render() override;

private:
    std::string texturePath;
    // Add other necessary members like texture ID, etc.
};

#endif // BACKGROUNDCHANNEL_H
