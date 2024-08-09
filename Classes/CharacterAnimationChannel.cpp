#include "../Headers/CharacterAnimationChannel.h"

CharacterAnimationChannel::CharacterAnimationChannel(const std::string& name)
    : Channel(name, ChannelType::CHARACTER_ANIMATION)
{
    // Initialize interpolated position, rotation, and scale
    interpolatedPosition = glm::vec3(0.0f);
    interpolatedRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    interpolatedScale = glm::vec3(1.0f);

    // Create and initialize the StickFigure character
    character = new StickFigure();
}

void CharacterAnimationChannel::update(float deltaTime) {
    // implementation
}

void CharacterAnimationChannel::render(const glm::mat4& view, const glm::mat4& projection) {
    if (!character) return;
    // implementation

    character->render(view, projection);
}

glm::mat4 CharacterAnimationChannel::getModelMatrix() const {
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    // Apply keyframe transformations first
    modelMatrix = glm::translate(modelMatrix, interpolatedPosition); // Apply translation
    modelMatrix *= glm::toMat4(interpolatedRotation); // Apply rotation
    modelMatrix = glm::scale(modelMatrix, interpolatedScale); // Apply scale

    if (name == "astronaut") {
        // Apply base scaling and rotation to standardize the model size and orientation because astronaut is too big
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.005f)); // Scale down by a factor of 0.1
        modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate 90 degrees around the X axis
        modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate -90 degrees around the Z axis
        modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate 180 degrees around the X axis
    }  

    return modelMatrix;
}




