#include "../Headers/VirtualCameraChannel.h"

std::vector<float> t_values;
std::vector<float> timeStamps;
std::vector<glm::vec3> drawnSpeedCurve;

VirtualCameraChannel::VirtualCameraChannel(const std::string& name)
    : Channel(name, ChannelType::VIRTUAL_CAMERA), isInitialized(false), pathShader(nullptr), isTraversalInProgress(false) {
}

void VirtualCameraChannel::startTraversal() {
    currentTime = 0.0f; // Reset the current time for traversal
    traversalComplete = false; // Reset the completion flag
    isTraversalInProgress = true; // Set the traversal in progress flag
    interpolatedKeyFrames = interpolateKeyFrames(); // Generate interpolated keyframes
}

float VirtualCameraChannel::easeInOutCubic(float t) {
    float easedValue = t < 0.5f ? 4.0f * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) / 2.0f;
    return easedValue;
}

// interpolate functions
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

glm::vec3 VirtualCameraChannel::bezierInterpolate(const std::vector<glm::vec3>& points, float t) const {
    if (points.size() == 1) {
        return points[0];
    }

    std::vector<glm::vec3> newPoints;
    for (size_t i = 0; i < points.size() - 1; ++i) {
        newPoints.push_back(glm::mix(points[i], points[i + 1], t));
    }

    return bezierInterpolate(newPoints, t);
}

glm::quat VirtualCameraChannel::bezierInterpolate(const std::vector<glm::quat>& points, float t) const {
    if (points.size() == 1) {
        return points[0];
    }

    std::vector<glm::quat> newPoints;
    for (size_t i = 0; i < points.size() - 1; ++i) {
        newPoints.push_back(glm::slerp(points[i], points[i + 1], t));
    }

    return bezierInterpolate(newPoints, t);
}

glm::vec3 VirtualCameraChannel::interpolatePosition(float time) const {
    if (keyFrames.empty()) return glm::vec3(0.0f);
    if (time <= keyFrames.front().timestamp) return keyFrames.front().position;
    if (time >= keyFrames.back().timestamp) return keyFrames.back().position;

    // Collect all control points from keyFrames
    std::vector<glm::vec3> controlPoints;
    for (const auto& keyFrame : keyFrames) {
        controlPoints.push_back(keyFrame.position);
    }

    // Calculate t for the whole curve
    float totalTime = keyFrames.back().timestamp - keyFrames.front().timestamp;
    float t = (time - keyFrames.front().timestamp) / totalTime;

    // Apply easing function
    t = easeInOutCubic(t);
    
    if (timeStamps.size() < 2 + (keyFrames.size() - 1) * (frameRate - 1))
    {
        t_values.push_back(t);
        timeStamps.push_back(time);
    }        

    return bezierInterpolate(controlPoints, t);
}

glm::quat VirtualCameraChannel::interpolateOrientation(float time) const {
    if (keyFrames.empty()) return glm::quat();
    if (time <= keyFrames.front().timestamp) return keyFrames.front().rotation;
    if (time >= keyFrames.back().timestamp) return keyFrames.back().rotation;

    // Collect all control points from keyFrames
    std::vector<glm::quat> controlPoints;
    for (const auto& keyFrame : keyFrames) {
        controlPoints.push_back(keyFrame.rotation);
    }

    // Calculate t for the whole curve
    float totalTime = keyFrames.back().timestamp - keyFrames.front().timestamp;
    float t = (time - keyFrames.front().timestamp) / totalTime;

    // Apply easing function
    t = easeInOutCubic(t);

    return bezierInterpolate(controlPoints, t);
}


void VirtualCameraChannel::update(float deltaTime) {
    if (isTraversalInProgress && !traversalComplete) {
        traversePath();
    }
}

// Refactored render function
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

    // Draw the path
    drawPath(view, projection, pathPositions);

    // Draw the keyframes
    drawKeyframes(view, projection, keyframePositions);

    // Draw the speed curve
    // Calculate rate of change
    std::vector<float> rate_of_change;
    if (t_values.size() > 1) {
        for (size_t i = 1; i < t_values.size(); ++i) {
            float rate = t_values[i] - t_values[i - 1];
            rate_of_change.push_back(rate);
        }
    }   

    std::vector<glm::vec3> speedCurvePositions;
    if (rate_of_change.size() > 0 && timeStamps.size() > 0) {
        for (size_t i = 0; i < rate_of_change.size(); ++i) {
            speedCurvePositions.push_back(glm::vec3(timeStamps[i], rate_of_change[i], 5.0f));
        }

        drawSpeedCurve(view, projection, speedCurvePositions);
	}
}

// Function to draw the path
void VirtualCameraChannel::drawPath(const glm::mat4& view, const glm::mat4& projection, const std::vector<glm::vec3>& pathPositions) {
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
}

// Function to draw the keyframes
void VirtualCameraChannel::drawKeyframes(const glm::mat4& view, const glm::mat4& projection, const std::vector<glm::vec3>& keyframePositions) {
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
}

void VirtualCameraChannel::drawSpeedCurve(const glm::mat4& view, const glm::mat4& projection, const std::vector<glm::vec3>& speedCurvePositions) {
    if (speedCurvePositions.empty()) return;

    if (!speedCurveDrawn) {
        speedCurveDrawn = true;
        drawnSpeedCurve.insert(drawnSpeedCurve.end(), speedCurvePositions.begin(), speedCurvePositions.end());
    }

    // Generate and bind VAO and VBO if they are not generated yet
    if (!glIsVertexArray(speedCurveVAO)) {
        glGenVertexArrays(1, &speedCurveVAO);
    }
    if (!glIsBuffer(speedCurveVBO)) {
        glGenBuffers(1, &speedCurveVBO);
    }

    // Create vertices for x and y axes
    std::vector<glm::vec3> axisVertices;
    glm::vec3 start = drawnSpeedCurve[0];
    axisVertices.push_back(start);               // Start of x-axis
    axisVertices.push_back(start + glm::vec3(drawnSpeedCurve.size() * 0.3f, 0.0f, 0.0f)); // End of x-axis
    axisVertices.push_back(start);               // Start of y-axis
    axisVertices.push_back(start + glm::vec3(0.0f, drawnSpeedCurve.size() * 0.05f, 0.0f)); // End of y-axis

    // Apply a scaling factor to the y-values for better visualization
    float yScaleFactor = 20.0f; // Adjust this value as needed for clarity

    // Create vertices for the speed curve with scaled y-values
    std::vector<glm::vec3> scaledSpeedCurve;
    for (const auto& point : drawnSpeedCurve) {
        glm::vec3 scaledPoint = point;
        scaledPoint.y *= yScaleFactor;
        scaledSpeedCurve.push_back(scaledPoint);
    }

    // Combine axis vertices and scaled speed curve positions
    std::vector<glm::vec3> combinedVertices = axisVertices;
    combinedVertices.insert(combinedVertices.end(), scaledSpeedCurve.begin(), scaledSpeedCurve.end());

    // Bind and upload combined vertices to the VBO
    glBindVertexArray(speedCurveVAO);
    glBindBuffer(GL_ARRAY_BUFFER, speedCurveVBO);
    glBufferData(GL_ARRAY_BUFFER, combinedVertices.size() * sizeof(glm::vec3), combinedVertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    // Use the speed curve shader program
    glUseProgram(speedCurveShader->ID);

    // Set view and projection matrices
    glUniformMatrix4fv(glGetUniformLocation(speedCurveShader->ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(speedCurveShader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // Draw the axes
    glDrawArrays(GL_LINES, 0, axisVertices.size());

    // Draw the speed curve
    glDrawArrays(GL_LINE_STRIP, axisVertices.size(), scaledSpeedCurve.size());

    // Unbind the VAO
    glBindVertexArray(0);
}


void VirtualCameraChannel::traversePath() {
    if (interpolatedKeyFrames.empty()) return;

    Camera& camera = Camera::getInstance();
    glm::vec3 cubePosition(0.0f, 0.0f, 0.0f);

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

void VirtualCameraChannel::printKeyframesWithInterpolations(std::vector<KeyFrame> interpolatedKeyFrames) {
    std::cout << "Base Keyframes:\n";
    for (const auto& kf : keyFrames) {
        std::cout << "Time: " << kf.timestamp
            << " Position: (" << kf.position.x << ", " << kf.position.y << ", " << kf.position.z << ")"
            << " Orientation: (" << kf.rotation.x << ", " << kf.rotation.y << ", " << kf.rotation.z << ", " << kf.rotation.w << ")"
            << " Scale: (" << kf.scale.x << ", " << kf.scale.y << ", " << kf.scale.z << ")\n";
    }

    // Print interpolated keyframes
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
        if (i == 0 || i == keyFrames.size() - 1) {
			interpolatedKeyFrames.push_back(keyFrames[i]);
		}
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

    // Initialize VAO and VBO for speed curve
    glGenVertexArrays(1, &speedCurveVAO);
    glGenBuffers(1, &speedCurveVBO);

    // Initialize the shaders
    pathShader = new Shader("../Shaders/path.vs", "../Shaders/path.fs");
    keyframeShader = new Shader("../Shaders/keyframe.vs", "../Shaders/keyframe.fs");
    speedCurveShader = new Shader("../Shaders/speed_curve.vs", "../Shaders/speed_curve.fs");

    isInitialized = true;
}
