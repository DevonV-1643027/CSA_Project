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
