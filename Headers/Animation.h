#pragma once
#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
#include <memory>
#include <string>
#include "../Headers/Channel.h"

class Animation {
public:
    Animation(const std::string& name) : name(name) {}

    void addChannel(std::shared_ptr<Channel> channel) {
        channels.push_back(channel);
    }

    void update(float deltaTime) {
        for (auto& channel : channels) {
            channel->update(deltaTime);
        }
    }

    void render() {
        for (auto& channel : channels) {
            channel->render();
        }
    }

    const std::string& getName() const { return name; }

private:
    std::string name;
    std::vector<std::shared_ptr<Channel>> channels;
};

#endif // ANIMATION_H
