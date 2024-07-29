#include "../Headers/BackgroundChannel.h"
#include <iostream>

BackgroundChannel::BackgroundChannel(const std::string& name)
    : Channel(name, BACKGROUND), texturePath("") {}

void BackgroundChannel::loadTexture(const std::string& filePath) {
    texturePath = filePath;
    // Load texture logic
    std::cout << "Loaded texture: " << texturePath << std::endl;
}

void BackgroundChannel::update(float deltaTime) {
    // Update logic for background
    std::cout << "Updating Background Channel: " << getName() << std::endl;
}

void BackgroundChannel::render() {
    // Check if texture is loaded
    if (texturePath.empty()) {
		std::cout << "No texture loaded for Background Channel: " << getName() << std::endl;
	}
    else {
        std::cout << "Rendering Background Channel: " << getName() << "with texture: " << texturePath << std::endl;
    }
    
}
