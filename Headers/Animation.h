#pragma once
#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
#include <memory>
#include <string>
#include "Channel.h"

class Animation {
public:
    Animation(const std::string& name);

    void addChannel(std::shared_ptr<Channel> channel);
    void removeChannel(const std::string& channelName);
    std::shared_ptr<Channel> getChannel(const std::string& channelName) const;
    void updateChannelName(const std::string& oldName, const std::string& newName);
    void update(float deltaTime);
    void render(const glm::mat4& view, const glm::mat4& projection);
    const std::string& getName() const;
    const std::vector<std::shared_ptr<Channel>>& getChannels() const;
    void swapChannels(size_t index1, size_t index2);

private:
    std::string name;
    std::vector<std::shared_ptr<Channel>> channels;
};

#endif // ANIMATION_H
