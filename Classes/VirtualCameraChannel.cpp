#include "../Headers/VirtualCameraChannel.h"

VirtualCameraChannel::VirtualCameraChannel(const std::string& name)
    : Channel(name, ChannelType::VIRTUAL_CAMERA), isInitialized(false), pathShader(nullptr) {
}

glm::vec3 VirtualCameraChannel::interpolatePosition(float time) const {
    if (keyFrames.empty()) return glm::vec3(0.0f);
    if (time <= keyFrames.front().timestamp) return keyFrames.front().position;
    if (time >= keyFrames.back().timestamp) return keyFrames.back().position;

    for (size_t i = 0; i < keyFrames.size() - 1; ++i) {
        if (time >= keyFrames[i].timestamp && time <= keyFrames[i + 1].timestamp) {
            return glm::mix(keyFrames[i].position, keyFrames[i + 1].position, (time - keyFrames[i].timestamp) / (keyFrames[i + 1].timestamp - keyFrames[i].timestamp));
        }
    }
    return glm::vec3(0.0f); // Should never reach here
}

glm::quat VirtualCameraChannel::interpolateOrientation(float time) const {
    if (keyFrames.empty()) return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    if (time <= keyFrames.front().timestamp) return keyFrames.front().rotation;
    if (time >= keyFrames.back().timestamp) return keyFrames.back().rotation;

    for (size_t i = 0; i < keyFrames.size() - 1; ++i) {
        if (time >= keyFrames[i].timestamp && time <= keyFrames[i + 1].timestamp) {
            return glm::slerp(keyFrames[i].rotation, keyFrames[i + 1].rotation, (time - keyFrames[i].timestamp) / (keyFrames[i + 1].timestamp - keyFrames[i].timestamp));
        }
    }
    return glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // Should never reach here
}

glm::vec3 VirtualCameraChannel::interpolateScale(float time) const {
    if (keyFrames.empty()) return glm::vec3(1.0f);
    if (time <= keyFrames.front().timestamp) return keyFrames.front().scale;
    if (time >= keyFrames.back().timestamp) return keyFrames.back().scale;

    for (size_t i = 0; i < keyFrames.size() - 1; ++i) {
        if (time >= keyFrames[i].timestamp && time <= keyFrames[i + 1].timestamp) {
            return glm::mix(keyFrames[i].scale, keyFrames[i + 1].scale, (time - keyFrames[i].timestamp) / (keyFrames[i + 1].timestamp - keyFrames[i].timestamp));
        }
    }
    return glm::vec3(1.0f); // Should never reach here
}

void VirtualCameraChannel::update(float deltaTime) {
    // Implementation for updating the camera based on interpolated values.
    // This can involve updating the camera's position, orientation, and scale in the scene.
}

void VirtualCameraChannel::render(const glm::mat4& view, const glm::mat4& projection) {
    // Print base keyframes: debugging purposes
    // printKeyframesWithInterpolations();

    if (!isInitialized) {
        initPathRendering();
    }

    std::vector<KeyFrame> interpolatedKeyFrames = interpolateKeyFrames();

    std::vector<glm::vec3> pathPositions;
    for (const auto& kf : interpolatedKeyFrames) {
        pathPositions.push_back(kf.position);
    }

    // Bind and upload path positions to the VBO
    glBindVertexArray(pathVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pathVBO);
    glBufferData(GL_ARRAY_BUFFER, pathPositions.size() * sizeof(glm::vec3), pathPositions.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    // Ensure the correct shader program is being used
    glUseProgram(pathShader->ID);

    // Set view and projection matrices
    glUniformMatrix4fv(glGetUniformLocation(pathShader->ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(pathShader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // Draw the path
    glDrawArrays(GL_LINE_STRIP, 0, pathPositions.size());

    // Unbind the vertex array object and shader program (clean up state)
    glBindVertexArray(0);
    glUseProgram(0);
}

void VirtualCameraChannel::printKeyframesWithInterpolations() {
    std::cout << "Base Keyframes:\n";
    for (const auto& kf : keyFrames) {
        std::cout << "Time: " << kf.timestamp
            << " Position: (" << kf.position.x << ", " << kf.position.y << ", " << kf.position.z << ")"
            << " Orientation: (" << kf.rotation.x << ", " << kf.rotation.y << ", " << kf.rotation.z << ", " << kf.rotation.w << ")"
            << " Scale: (" << kf.scale.x << ", " << kf.scale.y << ", " << kf.scale.z << ")\n";
    }

    // Print interpolated keyframes
    std::vector<KeyFrame> interpolatedKeyFrames = interpolateKeyFrames();
    if (interpolatedKeyFrames.size() < 2) {
        std::cout << "Not enough keyframes to interpolate.\n";
        return;
    }

    std::cout << "\nInterpolated Keyframes:\n";
    for (const auto& kf : interpolatedKeyFrames) {
        std::cout << "Time: " << kf.timestamp
            << " Position: (" << kf.position.x << ", " << kf.position.y << ", " << kf.position.z << ")"
            << " Orientation: (" << kf.rotation.x << ", " << kf.rotation.y << ", " << kf.rotation.z << ", " << kf.rotation.w << ")"
            << " Scale: (" << kf.scale.x << ", " << kf.scale.y << ", " << kf.scale.z << ")\n";
    }
}

std::vector<KeyFrame> VirtualCameraChannel::interpolateKeyFrames() const {
    std::vector<KeyFrame> interpolatedKeyFrames;

    if (keyFrames.empty()) return interpolatedKeyFrames;

    for (size_t i = 0; i < keyFrames.size() - 1; ++i) {
        interpolatedKeyFrames.push_back(keyFrames[i]);
        float startTime = keyFrames[i].timestamp;
        float endTime = keyFrames[i + 1].timestamp;
        float interval = (endTime - startTime) / frameRate;

        for (int j = 1; j < frameRate; ++j) {
            float currentTime = startTime + j * interval;
            glm::vec3 interpolatedPosition = interpolatePosition(currentTime);
            glm::quat interpolatedOrientation = interpolateOrientation(currentTime);
            glm::vec3 interpolatedScale = interpolateScale(currentTime);

            interpolatedKeyFrames.emplace_back(currentTime, interpolatedPosition, interpolatedOrientation, interpolatedScale);
        }
    }

    interpolatedKeyFrames.push_back(keyFrames.back());
    return interpolatedKeyFrames;
}

void VirtualCameraChannel::initPathRendering() {
    // Check if OpenGL context is initialized
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW or OpenGL context is not available." << std::endl;
        return;
    }

    // Initialize VAO and VBO
    glGenVertexArrays(1, &pathVAO);
    glGenBuffers(1, &pathVBO);

    // Initialize the shader
    pathShader = new Shader("../Shaders/path.vs", "../Shaders/path.fs");

    isInitialized = true;
}