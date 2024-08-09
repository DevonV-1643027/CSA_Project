#pragma once
#ifndef CHARACTERANIMATIONCHANNEL_H
#define CHARACTERANIMATIONCHANNEL_H

// for object loading I use the learnopengl implementation, all credits go to the authors
#include <learnopengl/model.h>

#include "Channel.h"
#include <glm/gtx/string_cast.hpp>

class CharacterAnimationChannel : public Channel {
public:
    CharacterAnimationChannel(const std::string& name) : Channel(name, ChannelType::CHARACTER_ANIMATION) {}
    void update(float deltaTime);
    void render(const glm::mat4& view, const glm::mat4& projection);

    void importObject(const std::string& path);
    void setupShader(const std::string& vertexPath, const std::string& fragmentPath);

private:
    Model* model = nullptr;
    Shader* shader = nullptr;

    glm::vec3 interpolatedPosition;
    glm::quat interpolatedRotation;
    glm::vec3 interpolatedScale;
    glm::mat4 getModelMatrix() const;
};

#endif // CHARACTERANIMATIONCHANNEL_H
