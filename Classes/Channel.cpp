#include "../Headers/Channel.h"

std::string Channel::getTypeString() const {
    switch (channelType) {
    case BACKGROUND: return "Background";
    case VIRTUAL_CAMERA: return "Virtual Camera";
    case STEP_AHEAD_ANIMATION: return "Step Ahead Animation";
    case CHARACTER_ANIMATION: return "Character Animation";
    default: return "Unknown";
    }
}

void Channel::swapKeyFrames(size_t index1, size_t index2) {
    if (index1 < keyFrames.size() && index2 < keyFrames.size()) {
        KeyFrame temp = keyFrames[index1];
        keyFrames[index1] = keyFrames[index2];
        keyFrames[index2] = temp;
    }
}

void Channel::updateKeyFrame(size_t index, const KeyFrame& keyFrame) {
    if (index < keyFrames.size()) {
        keyFrames[index] = keyFrame;
    }
}

void Channel::removeKeyFrame(size_t index) {
    if (index < keyFrames.size()) {
        keyFrames.erase(keyFrames.begin() + index);
    }
}

void Channel::loadKeyFramesFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open keyframe file: " << filePath << std::endl;
        return;
    }

    std::string line;
    KeyFrame keyFrame;
    bool firstKeyFrame = true;
    while (std::getline(file, line)) {
        // Remove leading and trailing whitespace
        line.erase(line.begin(), std::find_if(line.begin(), line.end(), [](unsigned char ch) {
            return !std::isspace(ch);
            }));
        line.erase(std::find_if(line.rbegin(), line.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
            }).base(), line.end());

        std::istringstream iss(line);

        if (line.find("KeyFrame") != std::string::npos) {
            if (!firstKeyFrame) {
                keyFrames.push_back(keyFrame);
                keyFrame = KeyFrame(); // Reset for the next keyframe
            }
            else {
                firstKeyFrame = false;
            }
        }
        else if (line.find("Control Point") != std::string::npos) {
            FFDControlPoint controlPoint;
            int controlPointIndex;
            int matched = sscanf_s(line.c_str(), "Control Point %d: Position (%f, %f, %f), Original Position (%f, %f, %f), Weight %f",
                &controlPointIndex,
                &controlPoint.position.x, &controlPoint.position.y, &controlPoint.position.z,
                &controlPoint.originalPosition.x, &controlPoint.originalPosition.y, &controlPoint.originalPosition.z,
                &controlPoint.weight);

            if (matched == 8) {
                keyFrame.ffdControlPoints.push_back(controlPoint);
            }
        }
        else if (line.find("Timestamp") != std::string::npos) {
            float timestamp;
            sscanf_s(line.c_str(), "Timestamp: %f", &timestamp);
            keyFrame.timestamp = timestamp;
        }
        else if (line.find("Position") != std::string::npos) {
            float x, y, z;
            sscanf_s(line.c_str(), "Position: (%f, %f, %f)", &x, &y, &z);
            keyFrame.position = glm::vec3(x, y, z);
        }
        else if (line.find("Orientation") != std::string::npos) {
            float x, y, z, w;
            sscanf_s(line.c_str(), "Orientation: (%f, %f, %f, %f)", &x, &y, &z, &w);
            keyFrame.rotation = glm::quat(w, x, y, z);
        }
        else if (line.find("Scale") != std::string::npos) {
            float x, y, z;
            sscanf_s(line.c_str(), "Scale: (%f, %f, %f)", &x, &y, &z);
            keyFrame.scale = glm::vec3(x, y, z);
        }
    }

    // Add the last keyframe if the file was read successfully
    if (!firstKeyFrame) {
        keyFrames.push_back(keyFrame);
    }

    file.close();
}
