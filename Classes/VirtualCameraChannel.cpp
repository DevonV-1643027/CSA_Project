#include "../Headers/VirtualCameraChannel.h"

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
    // Implementation for rendering the camera's trajectory or any visual aids for debugging.
    // This can involve drawing the path, keyframes, etc.

    // Print base keyframes: debugging purposes
    printKeyframesWithInterpolations();
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

