#pragma once
#ifndef CHANNEL_H
#define CHANNEL_H

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "../Headers/KeyFrame.h"

// Base class for different animation channels
class Channel {
public:
    Channel(const std::string& name) : name(name) {}
    virtual ~Channel() {}

    // Virtual methods to be implemented by derived classes
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;

    // Common methods
    const std::string& getName() const { return name; }
    void setName(const std::string& newName) { name = newName; }

    void addKeyFrame(const KeyFrame& keyFrame) { keyFrames.push_back(keyFrame); }
    const std::vector<KeyFrame>& getKeyFrames() const { return keyFrames; }

protected:
    std::string name;
    std::vector<KeyFrame> keyFrames;  // Store key frames
};

#endif // CHANNEL_H

