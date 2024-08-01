#include "../Headers/VirtualCameraChannel.h"

VirtualCameraChannel::VirtualCameraChannel(const std::string& name)
    : Channel(name, ChannelType::VIRTUAL_CAMERA), isInitialized(false), pathShader(nullptr), isTraversalInProgress(false) {
}

void VirtualCameraChannel::startTraversal() {
    currentTime = 0.0f; // Reset the current time for traversal
    traversalComplete = false; // Reset the completion flag
    isTraversalInProgress = true; // Set the traversal in progress flag
    interpolatedKeyFrames = interpolateKeyFrames(); // Generate interpolated keyframes
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
    if (isTraversalInProgress && !traversalComplete) {
        traversePath();
    }
    else if (!traversalComplete) {
        // Allow user control if traversal is not in progress
        currentTime += deltaTime;

        // Guard clause for empty keyFrames
        if (keyFrames.empty()) return;

        KeyFrame startFrame = keyFrames.front();
        KeyFrame endFrame = keyFrames.back();

        for (size_t i = 0; i < keyFrames.size() - 1; ++i) {
            if (keyFrames[i].timestamp <= currentTime && keyFrames[i + 1].timestamp > currentTime) {
                startFrame = keyFrames[i];
                endFrame = keyFrames[i + 1];
                break;
            }
        }

        float factor = (currentTime - startFrame.timestamp) / (endFrame.timestamp - startFrame.timestamp);
        glm::vec3 interpolatedPosition = glm::mix(startFrame.position, endFrame.position, factor);
        glm::quat interpolatedRotation = glm::slerp(startFrame.rotation, endFrame.rotation, factor);

        Camera& camera = Camera::getInstance();
        camera.Position = interpolatedPosition;

        // Make the camera look at the cube
        glm::vec3 cubePosition(0.0f, 0.0f, 0.0f); // Assuming the cube is at the origin
        camera.Front = glm::normalize(cubePosition - camera.Position);
        camera.Right = glm::normalize(glm::cross(camera.Front, camera.WorldUp));
        camera.Up = glm::normalize(glm::cross(camera.Right, camera.Front));
    }
}

void VirtualCameraChannel::render(const glm::mat4& view, const glm::mat4& projection) {
    if (!isInitialized) {
        initPathRendering();
    }

    std::vector<KeyFrame> interpolatedKeyFrames = interpolateKeyFrames();

    std::vector<glm::vec3> pathPositions;
    std::vector<glm::vec3> keyframePositions;
    for (const auto& kf : interpolatedKeyFrames) {
        pathPositions.push_back(kf.position);
    }
    for (const auto& kf : keyFrames) {
        keyframePositions.push_back(kf.position);
    }

    // Bind and upload path positions to the VBO
    glBindVertexArray(pathVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pathVBO);
    glBufferData(GL_ARRAY_BUFFER, pathPositions.size() * sizeof(glm::vec3), pathPositions.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    // Use the path shader program
    glUseProgram(pathShader->ID);

    // Set view and projection matrices
    glUniformMatrix4fv(glGetUniformLocation(pathShader->ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(pathShader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // Draw the path
    glDrawArrays(GL_LINE_STRIP, 0, pathPositions.size());

    // Unbind the path VAO
    glBindVertexArray(0);

    // Bind and upload keyframe positions to the VBO
    glBindVertexArray(keyframeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, keyframeVBO);
    glBufferData(GL_ARRAY_BUFFER, keyframePositions.size() * sizeof(glm::vec3), keyframePositions.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    // Enable point size
    glEnable(GL_PROGRAM_POINT_SIZE);

    // Use the keyframe shader program
    glUseProgram(keyframeShader->ID);

    // Set view and projection matrices
    glUniformMatrix4fv(glGetUniformLocation(keyframeShader->ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(keyframeShader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // Draw the keyframes as points
    glDrawArrays(GL_POINTS, 0, keyframePositions.size());

    // Unbind the keyframe VAO and shader program
    glBindVertexArray(0);
    glUseProgram(0);

    // Start traversal if not already in progress
    if (!isTraversalInProgress && !traversalComplete) {
        startTraversal();
    }
}

void VirtualCameraChannel::traversePath() {
    if (interpolatedKeyFrames.empty()) return;

    Camera& camera = Camera::getInstance();
    glm::vec3 cubePosition(0.0f, 0.0f, 0.0f); // Assuming the cube is at the origin

    if (currentTime >= interpolatedKeyFrames.back().timestamp) {
        traversalComplete = true;
        isTraversalInProgress = false;
        return;
    }

    for (const auto& keyFrame : interpolatedKeyFrames) {
        if (keyFrame.timestamp > currentTime) {
            camera.Position = keyFrame.position;

            // Make the camera look at the cube
            camera.Front = glm::normalize(cubePosition - camera.Position);
            camera.Right = glm::normalize(glm::cross(camera.Front, camera.WorldUp));
            camera.Up = glm::normalize(glm::cross(camera.Right, camera.Front));

            break;
        }
    }

    currentTime += 0.016f; // Simulate time progression, equivalent to 60 FPS
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

    // Initialize VAO and VBO for path
    glGenVertexArrays(1, &pathVAO);
    glGenBuffers(1, &pathVBO);

    // Initialize VAO and VBO for keyframes
    glGenVertexArrays(1, &keyframeVAO);
    glGenBuffers(1, &keyframeVBO);

    // Initialize the shaders
    pathShader = new Shader("../Shaders/path.vs", "../Shaders/path.fs");
    keyframeShader = new Shader("../Shaders/keyframe.vs", "../Shaders/keyframe.fs");

    isInitialized = true;
}
