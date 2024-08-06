#pragma once
#ifndef KEYFRAME_H
#define KEYFRAME_H

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>

// Define the FFDControlPoint structure
struct FFDControlPoint {
    glm::vec3 position;        // Current position of the control point
    glm::vec3 originalPosition; // Original position of the control point
    float weight;              // Influence weight of the control point

    FFDControlPoint(glm::vec3 pos, glm::vec3 origPos, float w)
        : position(pos), originalPosition(origPos), weight(w) {}
};

// Define the KeyFrame structure
struct KeyFrame {
    float timestamp;          // Time at which this key frame occurs
    glm::vec3 position;       // Position of the object at this key frame
    glm::quat rotation;       // Rotation of the object at this key frame
    glm::vec3 scale;          // Scale of the object at this key frame
    std::vector<FFDControlPoint> ffdControlPoints; // List of FFD control points

    KeyFrame(float time, glm::vec3 pos, glm::quat rot, glm::vec3 scl)
        : timestamp(time), position(pos), rotation(rot), scale(scl) {}
};

#endif // KEYFRAME_H
