#include "../Headers/CharacterAnimationChannel.h"

void CharacterAnimationChannel::update(float deltaTime) {
    if (!keyFrames.empty()) {
        // Assuming keyFrames[0] holds the first keyframe
        const auto& firstKeyFrame = keyFrames[0];

        // Initialize interpolated values based on the first keyframe
        interpolatedPosition = firstKeyFrame.position;
        interpolatedRotation = firstKeyFrame.rotation;
        interpolatedScale = firstKeyFrame.scale;
    }
}

void CharacterAnimationChannel::render(const glm::mat4& view, const glm::mat4& projection) {
    if (!shader || !model) return;

    shader->use();

    shader->setMat4("view", view);
    shader->setMat4("projection", projection);

    glm::mat4 modelMatrix = getModelMatrix();
    shader->setMat4("model", modelMatrix);

    // Set light properties
    shader->setVec3("lightPos", glm::vec3(10.0f, 10.0f, 10.0f)); // Example light position
    shader->setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f)); // White light

    model->Draw(*shader);
}

void CharacterAnimationChannel::importObject(const std::string& path) {
    if (model) {
        delete model;
    }
    model = new Model(path.c_str());
}

void CharacterAnimationChannel::setupShader(const std::string& vertexPath, const std::string& fragmentPath) {
    if (shader) {
        delete shader;
    }
    shader = new Shader(("../Shaders/" + vertexPath).c_str(), ("../Shaders/" + fragmentPath).c_str());
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




