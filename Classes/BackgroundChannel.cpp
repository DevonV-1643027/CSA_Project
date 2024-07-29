#include "../Headers/BackgroundChannel.h"
#include <iostream>

BackgroundChannel::BackgroundChannel(const std::string& name)
    : Channel(name, BACKGROUND), texturePath("") {}

void BackgroundChannel::loadTexture(const std::string& filePath) {
    texturePath = filePath;
    // Load texture logic
    std::cout << "Loaded texture: " << filePath << std::endl;
}

void BackgroundChannel::update(float deltaTime) {
    // Update logic for background
    std::cout << "Updating Background Channel: " << getName() << std::endl;
}

void BackgroundChannel::render() {
    // Render background logic
    std::cout << "Rendering Background Channel: " << getName() << std::endl;
}
