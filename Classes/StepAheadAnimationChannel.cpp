#include "../Headers/StepAheadAnimationChannel.h"

StepAheadAnimationChannel::StepAheadAnimationChannel(const std::string& name)
    : Channel(name, STEP_AHEAD_ANIMATION) {}

void StepAheadAnimationChannel::update(float deltaTime) {
    // Implement the update logic here
}

void StepAheadAnimationChannel::render(const glm::mat4& view, const glm::mat4& projection) {
    // Use the shader program
    shader->use();

    // Set the view and projection matrix uniforms
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);

    // Retrieve the current keyframe (assuming a method getCurrentKeyFrame exists)
    KeyFrame keyFrame = keyFrames[0];

    // Calculate the model matrix from keyframe transformation
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, keyFrame.position); // Apply translation
    modelMatrix *= glm::toMat4(keyFrame.rotation); // Apply rotation
    modelMatrix = glm::scale(modelMatrix, keyFrame.scale); // Apply scale

    // Set the model matrix uniform
    shader->setMat4("model", modelMatrix);

    // Render back faces
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    model->Draw(*shader);

    // Render front faces
    glCullFace(GL_BACK);
    model->Draw(*shader);

    // Optionally disable face culling if needed
    glDisable(GL_CULL_FACE);
}

void StepAheadAnimationChannel::importObject(const std::string& path) {
    model = new Model(path.c_str());
}

void StepAheadAnimationChannel::setupShader(const std::string& vertexPath, const std::string& fragmentPath) {
    shader = new Shader(("../Shaders/" + vertexPath).c_str(), ("../Shaders/" + fragmentPath).c_str());
}


