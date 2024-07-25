#pragma once
#ifndef KEYFRAME_H
#define KEYFRAME_H

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

struct KeyFrame {
    float timestamp;          // Time at which this key frame occurs
    glm::vec3 position;       // Position of the object at this key frame
    glm::quat rotation;       // Rotation of the object at this key frame
    glm::vec3 scale;          // Scale of the object at this key frame

    KeyFrame(float time, glm::vec3 pos, glm::quat rot, glm::vec3 scl)
        : timestamp(time), position(pos), rotation(rot), scale(scl) {}
};

#endif // KEYFRAME_H
