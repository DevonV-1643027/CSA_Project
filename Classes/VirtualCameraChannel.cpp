#include "../Headers/VirtualCameraChannel.h"

void VirtualCameraChannel::addKeyFrame(float time, const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale) {
    KeyFrame kf = { time, position, orientation, scale };
    keyFrames.push_back(kf);
    std::sort(keyFrames.begin(), keyFrames.end(), [](const KeyFrame& a, const KeyFrame& b) { return a.timestamp < b.timestamp; });
}

VirtualCameraChannel::KeyFrame VirtualCameraChannel::getKeyFrame(float time) const {
    for (const auto& kf : keyFrames) {
        if (kf.timestamp == time) {
            return kf;
        }
    }
    throw std::runtime_error("Key frame not found");
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
    // Implementation for rendering the camera's trajectory or any visual aids for debugging.
    // This can involve drawing the path, keyframes, etc.
    // For now: testing interpolation
    // Print base keyframes
    std::cout << "Base Keyframes:\n";
    for (const auto& kf : keyFrames) {
        std::cout << "Time: " << kf.timestamp
            << " Position: (" << kf.position.x << ", " << kf.position.y << ", " << kf.position.z << ")"
            << " Orientation: (" << kf.rotation.x << ", " << kf.rotation.y << ", " << kf.rotation.z << ", " << kf.rotation.w << ")"
            << " Scale: (" << kf.scale.x << ", " << kf.scale.y << ", " << kf.scale.z << ")\n";
    }

    // Print interpolated keyframes
    if (keyFrames.size() < 2) {
        std::cout << "Not enough keyframes to interpolate.\n";
        return;
    }

    std::cout << "\nInterpolated Keyframes:\n";
    const int steps = 10; // Number of interpolated steps between each keyframe
    for (size_t i = 0; i < keyFrames.size() - 1; ++i) {
        for (int j = 1; j < steps; ++j) {
            float t = keyFrames[i].timestamp + j * (keyFrames[i + 1].timestamp - keyFrames[i].timestamp) / steps;
            glm::vec3 interpolatedPosition = interpolatePosition(t);
            glm::quat interpolatedOrientation = interpolateOrientation(t);
            glm::vec3 interpolatedScale = interpolateScale(t);
            std::cout << "Time: " << t
                << " Position: (" << interpolatedPosition.x << ", " << interpolatedPosition.y << ", " << interpolatedPosition.z << ")"
                << " Orientation: (" << interpolatedOrientation.x << ", " << interpolatedOrientation.y << ", " << interpolatedOrientation.z << ", " << interpolatedOrientation.w << ")"
                << " Scale: (" << interpolatedScale.x << ", " << interpolatedScale.y << ", " << interpolatedScale.z << ")\n";
        }
    }
}
