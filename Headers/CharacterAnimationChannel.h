#pragma once
#ifndef CHARACTERANIMATIONCHANNEL_H
#define CHARACTERANIMATIONCHANNEL_H

#include "Channel.h"

class CharacterAnimationChannel : public Channel {
public:
    CharacterAnimationChannel(const std::string& name) : Channel(name, ChannelType::CHARACTER_ANIMATION) {}
    void update(float deltaTime) override {
        // Basic update logic for CharacterAnimationChannel
    }
    virtual void render(const glm::mat4& view, const glm::mat4& projection) override {
		// Basic render logic for CharacterAnimationChannel
	}
};

#endif // CHARACTERANIMATIONCHANNEL_H
