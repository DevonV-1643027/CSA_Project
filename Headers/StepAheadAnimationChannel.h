#pragma once
#ifndef STEPAHEADANIMATIONCHANNEL_H
#define STEPAHEADANIMATIONCHANNEL_H

#include "Channel.h"
#include <fstream>
#include <sstream>
#include <iostream>

class StepAheadAnimationChannel : public Channel {
public:
    StepAheadAnimationChannel(const std::string& name) : Channel(name, ChannelType::STEP_AHEAD_ANIMATION) {}

    void update(float deltaTime) override;
    void render(const glm::mat4& view, const glm::mat4& projection) override;
    bool importObject(const std::string& filePath); // Function to import an OBJ file

private:
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
};

#endif // STEPAHEADANIMATIONCHANNEL_H
