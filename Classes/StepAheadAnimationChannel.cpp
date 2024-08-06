#include "../Headers/StepAheadAnimationChannel.h"

StepAheadAnimationChannel::StepAheadAnimationChannel(const std::string& name)
    : Channel(name, STEP_AHEAD_ANIMATION), model(nullptr), shader(nullptr), currentTime(0.0f) {

    if (name == "sun") {
        lightPosition = glm::vec3(0.0f, 100.0f, 100.0f); // Light source for the sun, far and bright
    }
    else if (name == "earth") {
        lightPosition = glm::vec3(10.0f, 0.0f, 0.0f); // Light source for the earth
    }
	else {
		lightPosition = glm::vec3(0.0f, 0.0f, 0.0f); // Default light position
	}

    viewPosition = glm::vec3(0.0f, 0.0f, -2.0f); // Default view position

    if (!keyFrames.empty()) {
        interpolatedPosition = keyFrames.front().position;
        interpolatedRotation = keyFrames.front().rotation;
        interpolatedScale = keyFrames.front().scale;
    }
}

void StepAheadAnimationChannel::update(float deltaTime) {
    currentTime += deltaTime;
    interpolateControlPoints();
    interpolateKeyFrame();
    applyFFD();
}

void StepAheadAnimationChannel::render(const glm::mat4& view, const glm::mat4& projection) {
    if (!shader || !model) return;

    // Use the shader program
    shader->use();

    // Set the view and projection matrix uniforms
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);

    // Get the interpolated model matrix
    glm::mat4 modelMatrix = getModelMatrix();

    // Set the model matrix uniform
    shader->setMat4("model", modelMatrix);

    // Set the light and view positions
    shader->setVec3("lightPos", lightPosition);
    shader->setVec3("viewPos", viewPosition);

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
    if (keyFrames.empty()) return;

    if (keyFrames.size() < 2) {
        // If there is only one keyframe, use its control points directly
        currentControlPoints = keyFrames.front().ffdControlPoints;
        return;
    }

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

void StepAheadAnimationChannel::applyFFD() {
    if (!model) return;

    // Loop through each mesh in the model
    for (auto& mesh : model->meshes) {
        // Loop through each vertex in the mesh
        for (auto& vertex : mesh.vertices) {
            glm::vec3 newPosition = vertex.Position;
            // Apply FFD based on the current control points and weight
            for (const auto& cp : currentControlPoints) {
                float influence = cp.weight;
                newPosition += (cp.position - cp.originalPosition) * influence;
            }
            vertex.Position = newPosition;
        }
    }
}

void StepAheadAnimationChannel::interpolateKeyFrame() {
    if (keyFrames.empty()) return;

    if (keyFrames.size() < 2) {
        // If there is only one keyframe, use its transformations directly
        interpolatedPosition = keyFrames.front().position;
        interpolatedRotation = keyFrames.front().rotation;
        interpolatedScale = keyFrames.front().scale;

        return;
    }

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

    interpolatedPosition = glm::mix(prevKeyFrame->position, nextKeyFrame->position, t);
    interpolatedRotation = glm::slerp(prevKeyFrame->rotation, nextKeyFrame->rotation, t);
    interpolatedScale = glm::mix(prevKeyFrame->scale, nextKeyFrame->scale, t);
}

glm::mat4 StepAheadAnimationChannel::getModelMatrix() const {
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, interpolatedPosition); // Apply translation
    modelMatrix *= glm::toMat4(interpolatedRotation); // Apply rotation
    modelMatrix = glm::scale(modelMatrix, interpolatedScale); // Apply scale
    return modelMatrix;
}
