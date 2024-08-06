#include "../Headers/StepAheadAnimationChannel.h"

StepAheadAnimationChannel::StepAheadAnimationChannel(const std::string& name)
    : Channel(name, STEP_AHEAD_ANIMATION) {}

void StepAheadAnimationChannel::update(float deltaTime) {
    currentTime += deltaTime;
    interpolateControlPoints();
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
    if (model) {
        delete model;
    }
    model = new Model(path.c_str());
}

void StepAheadAnimationChannel::setupShader(const std::string& vertexPath, const std::string& fragmentPath) {
    if (shader) {
        delete shader;
    }
    shader = new Shader(("../Shaders/" + vertexPath).c_str(), ("../Shaders/" + fragmentPath).c_str());
}

void StepAheadAnimationChannel::interpolateControlPoints() {
    if (keyFrames.size() < 2) return;

    KeyFrame* prevKeyFrame = nullptr;
    KeyFrame* nextKeyFrame = nullptr;

    for (size_t i = 0; i < keyFrames.size() - 1; ++i) {
        if (currentTime >= keyFrames[i].timestamp && currentTime <= keyFrames[i + 1].timestamp) {
            prevKeyFrame = &keyFrames[i];
            nextKeyFrame = &keyFrames[i + 1];
            break;
        }
    }

    if (!prevKeyFrame || !nextKeyFrame) return;

    float t = (currentTime - prevKeyFrame->timestamp) / (nextKeyFrame->timestamp - prevKeyFrame->timestamp);

    currentControlPoints.resize(prevKeyFrame->ffdControlPoints.size());
    for (size_t i = 0; i < prevKeyFrame->ffdControlPoints.size(); ++i) {
        currentControlPoints[i].position = glm::mix(prevKeyFrame->ffdControlPoints[i].position,
            nextKeyFrame->ffdControlPoints[i].position, t);
        /*
        // Debugging: Print interpolated control points, saved for later
        std::cout << "Control Point " << i << ": ("
            << currentControlPoints[i].position.x << ", "
            << currentControlPoints[i].position.y << ", "
            << currentControlPoints[i].position.z << ")\n";
        */        
    }
}



