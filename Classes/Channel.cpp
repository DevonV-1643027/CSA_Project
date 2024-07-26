#include "../Headers/Channel.h"

std::string Channel::getTypeString() const {
    switch (channelType) {
    case BACKGROUND: return "Background";
    case VIRTUAL_CAMERA: return "Virtual Camera";
    case STEP_AHEAD_ANIMATION: return "Step Ahead Animation";
    case CHARACTER_ANIMATION: return "Character Animation";
    default: return "Unknown";
    }
}

void Channel::swapKeyFrames(size_t index1, size_t index2) {
    if (index1 < keyFrames.size() && index2 < keyFrames.size()) {
        KeyFrame temp = keyFrames[index1];
        keyFrames[index1] = keyFrames[index2];
        keyFrames[index2] = temp;
    }
}

void Channel::updateKeyFrame(size_t index, const KeyFrame& keyFrame) {
    if (index < keyFrames.size()) {
        keyFrames[index] = keyFrame;
    }
}

void Channel::removeKeyFrame(size_t index) {
    if (index < keyFrames.size()) {
        keyFrames.erase(keyFrames.begin() + index);
    }
}