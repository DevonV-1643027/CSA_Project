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
    if (animationFinished) return;

    if (!stored) {
		storeOriginalPositions();
        stored = true;
	}

    currentTime += deltaTime;

    if (currentTime >= keyFrames.back().timestamp) {
        currentTime = keyFrames.back().timestamp;
        interpolateControlPoints();
        interpolateKeyFrame();
        applyFFD();

        // Set to the first keyframe's position, rotation, and scale
        interpolatedPosition = keyFrames.front().position;
        interpolatedRotation = keyFrames.front().rotation;
        interpolatedScale = keyFrames.front().scale;

        // Reset model's meshes to the original positions of the first keyframe's control points
        for (auto& mesh : model->meshes) {
            size_t numControlPoints = keyFrames.front().ffdControlPoints.size();
            for (size_t i = 0; i < mesh.vertices.size(); ++i) {
                if (i < numControlPoints) {
                    mesh.vertices[i].Position = keyFrames.front().ffdControlPoints[i].originalPosition;
                }
                else {
                    // If there are more vertices than control points, ensure they are reset too
                    mesh.vertices[i] = originalPositions[i];
                }
            }
            // Update the mesh data
            mesh.updateMeshData();
        }

        // Mark animation as finished
        animationFinished = true;
    }
    else {
        interpolateControlPoints();
        interpolateKeyFrame();
        applyFFD();
    }
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

        // Keep original positions and weights constant
        currentControlPoints[i].originalPosition = prevKeyFrame->ffdControlPoints[i].originalPosition;
        currentControlPoints[i].weight = prevKeyFrame->ffdControlPoints[i].weight;
    }
}

void StepAheadAnimationChannel::applyFFD() {
    if (!model) {
        std::cout << "Model not loaded, FFD cannot be applied." << std::endl;
        return;
    }

    glm::vec3 centerOfMassBefore = glm::vec3(0.0f);
    glm::vec3 centerOfMassAfter = glm::vec3(0.0f);
    size_t totalVertices = 0;

    // Calculate initial center of mass
    for (const auto& mesh : model->meshes) {
        totalVertices += mesh.vertices.size();
        for (const auto& vertex : mesh.vertices) {
            centerOfMassBefore += vertex.Position;
        }
    }
    centerOfMassBefore /= static_cast<float>(totalVertices);

    // Apply FFD to the model vertices using control points
    for (auto& mesh : model->meshes) {
        for (size_t i = 0; i < mesh.vertices.size(); ++i) {
            auto& vertex = mesh.vertices[i];
            glm::vec3 originalPosition = vertex.Position;
            glm::vec3 displacement = glm::vec3(0.0f);
            float totalWeight = 0.0f;

            for (const auto& cp : currentControlPoints) {
                float distance = glm::length(originalPosition - cp.originalPosition);
                float weight = cp.weight / (distance + 1.0f);
                displacement += weight * (cp.position - cp.originalPosition);
                totalWeight += weight;
            }

            if (totalWeight > 0.0f) {
                displacement /= totalWeight;
            }

            glm::vec3 newPosition = originalPosition + displacement;
            vertex.Position = newPosition;
            centerOfMassAfter += newPosition;
        }
    }

    // Calculate new center of mass
    centerOfMassAfter /= static_cast<float>(totalVertices);

    // Adjust vertices to maintain the same center of mass
    glm::vec3 correction = centerOfMassBefore - centerOfMassAfter;

    for (auto& mesh : model->meshes) {
        for (auto& vertex : mesh.vertices) {
            vertex.Position += correction;
        }
        mesh.updateMeshData();
    }
}






void StepAheadAnimationChannel::interpolateKeyFrame() {
    if (keyFrames.size() < 2) return;

    const KeyFrame* prevKeyFrame = nullptr;
    const KeyFrame* nextKeyFrame = nullptr;

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

void StepAheadAnimationChannel::storeOriginalPositions() {
    originalPositions.clear();
    for (const auto& mesh : model->meshes) {
        for (const auto& vertex : mesh.vertices) {
            originalPositions.push_back(vertex);
        }
    }
}
