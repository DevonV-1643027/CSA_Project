#pragma once
#ifndef CHANNEL_H
#define CHANNEL_H

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "../Headers/KeyFrame.h"

enum ChannelType {
    BACKGROUND,
    VIRTUAL_CAMERA,
    STEP_AHEAD_ANIMATION,
    CHARACTER_ANIMATION
};

// Base class for different animation channels
class Channel {
public:
    Channel(const std::string& name, ChannelType type) : name(name), channelType(type) {}
    virtual ~Channel() {}

    // Virtual methods to be implemented by derived classes
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;

    // Common methods
    const std::string& getName() const { return name; }
    void setName(const std::string& newName) { name = newName; }
    ChannelType getType() const { return channelType; }
    std::string getTypeString() const; // Declaration of the function
    void addKeyFrame(const KeyFrame& keyFrame) { keyFrames.push_back(keyFrame); }
    const std::vector<KeyFrame>& getKeyFrames() const { return keyFrames; }

    void swapKeyFrames(size_t index1, size_t index2);
    void updateKeyFrame(size_t index, const KeyFrame& keyFrame);
    void removeKeyFrame(size_t index);

protected:
    std::string name;
    ChannelType channelType;
    std::vector<KeyFrame> keyFrames;  // Store key frames
};

#endif // CHANNEL_H
