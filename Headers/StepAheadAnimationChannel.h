#pragma once
#ifndef STEPAHEADANIMATIONCHANNEL_H
#define STEPAHEADANIMATIONCHANNEL_H

#include "Channel.h"

class StepAheadAnimationChannel : public Channel {
public:
    StepAheadAnimationChannel(const std::string& name) : Channel(name, ChannelType::STEP_AHEAD_ANIMATION) {}
    void update(float deltaTime) override {
        // Basic update logic for StepAheadAnimationChannel
    }
    virtual void render(const glm::mat4& view, const glm::mat4& projection) override {
        // Basic render logic for CharacterAnimationChannel
    }
};

#endif // STEPAHEADANIMATIONCHANNEL_H
