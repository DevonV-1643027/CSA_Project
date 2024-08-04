#include "../Headers/Animation.h"
#include <algorithm>

Animation::Animation(const std::string& name) : name(name) {}

void Animation::addChannel(std::shared_ptr<Channel> channel) {
    channels.push_back(channel);
}

void Animation::removeChannel(const std::string& channelName) {
    channels.erase(std::remove_if(channels.begin(), channels.end(),
        [&channelName](const std::shared_ptr<Channel>& channel) {
            return channel->getName() == channelName;
        }), channels.end());
}

std::shared_ptr<Channel> Animation::getChannel(const std::string& channelName) const {
    auto it = std::find_if(channels.begin(), channels.end(),
        [&channelName](const std::shared_ptr<Channel>& channel) {
            return channel->getName() == channelName;
        });
    return (it != channels.end()) ? *it : nullptr;
}

void Animation::updateChannelName(const std::string& oldName, const std::string& newName) {
    auto channel = getChannel(oldName);
    if (channel) {
        channel->setName(newName);
    }
}

void Animation::update(float deltaTime) {
    for (auto& channel : channels) {
        channel->update(deltaTime);            
    }
}

void Animation::render(const glm::mat4& view, const glm::mat4& projection) {
    for (auto& channel : channels) {
        if (!channel->isActive) {
            continue; // Skip rendering if the channel is not active
        }
        // Check for background channel
        if (channel->getType() == 0) {
            channel->render(skyboxView, projection);
        }
        else {
            channel->render(view, projection);
        }
    }
}

const std::string& Animation::getName() const {
    return name;
}

const std::vector<std::shared_ptr<Channel>>& Animation::getChannels() const {
    return channels;
}

void Animation::swapChannels(size_t index1, size_t index2) {
    if (index1 < channels.size() && index2 < channels.size()) {
        std::swap(channels[index1], channels[index2]);
    }
}
