#pragma once
#ifndef CHARACTERANIMATIONCHANNEL_H
#define CHARACTERANIMATIONCHANNEL_H

#define NOMINMAX
#include <Windows.h>

#include "Channel.h"
#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <fstream>

#include "StickFigure.h"


class CharacterAnimationChannel : public Channel {
public:
    CharacterAnimationChannel(const std::string& name);
    void update(float deltaTime);
    void render(const glm::mat4& view, const glm::mat4& projection);

private:
    glm::vec3 interpolatedPosition;
    glm::quat interpolatedRotation;
    glm::vec3 interpolatedScale;
    glm::mat4 getModelMatrix() const;

    StickFigure* character;
};

#endif // CHARACTERANIMATIONCHANNEL_H
