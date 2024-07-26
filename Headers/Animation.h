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

    void removeChannel(const std::string& channelName) {
        channels.erase(std::remove_if(channels.begin(), channels.end(),
            [&channelName](const std::shared_ptr<Channel>& channel) {
                return channel->getName() == channelName;
            }), channels.end());
    }

    std::shared_ptr<Channel> getChannel(const std::string& channelName) const {
        auto it = std::find_if(channels.begin(), channels.end(),
            [&channelName](const std::shared_ptr<Channel>& channel) {
                return channel->getName() == channelName;
            });
        return (it != channels.end()) ? *it : nullptr;
    }

    void updateChannelName(const std::string& oldName, const std::string& newName) {
        auto channel = getChannel(oldName);
        if (channel) {
            channel->setName(newName);
        }
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

    const std::vector<std::shared_ptr<Channel>>& getChannels() const {
        return channels;
    }

private:
    std::string name;
    std::vector<std::shared_ptr<Channel>> channels;
};

#endif // ANIMATION_H
