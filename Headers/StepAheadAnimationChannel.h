#pragma once
#ifndef STEPAHEADANIMATIONCHANNEL_H
#define STEPAHEADANIMATIONCHANNEL_H

#include "../Headers/Channel.h"

class StepAheadAnimationChannel : public Channel {
public:
    StepAheadAnimationChannel(const std::string& name) : Channel(name, ChannelType::STEP_AHEAD_ANIMATION) {}
    void update(float deltaTime) override {
        // Basic update logic for StepAheadAnimationChannel
    }
    void render() override {
        // Basic render logic for StepAheadAnimationChannel
    }
};

#endif // STEPAHEADANIMATIONCHANNEL_H
