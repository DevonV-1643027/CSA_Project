#pragma once
#ifndef CHANNEL_H
#define CHANNEL_H

#include <string>
#include <vector>
#include <glm/glm.hpp>

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
    void addKeyFrame(const glm::mat4& keyFrame) { keyFrames.push_back(keyFrame); }
    const std::vector<glm::mat4>& getKeyFrames() const { return keyFrames; }

protected:
    std::string name;
    std::vector<glm::mat4> keyFrames;  // Store key frames as transformation matrices
};

#endif // CHANNEL_H
