#ifndef STEPAHEADANIMATIONCHANNEL_H
#define STEPAHEADANIMATIONCHANNEL_H

// for object loading I use the learnopengl implementation, all credits go to the authors
#include <learnopengl/model.h>

#include "Channel.h"
#include <iostream> // Debugging

class StepAheadAnimationChannel : public Channel {
public:
    StepAheadAnimationChannel(const std::string& name);

    void update(float deltaTime) override;
    void render(const glm::mat4& view, const glm::mat4& projection);

    void importObject(const std::string& path);
    void setupShader(const std::string& vertexPath, const std::string& fragmentPath);

private:
    Model* model = nullptr;
    Shader* shader = nullptr;
    std::vector<FFDControlPoint> currentControlPoints;

    void interpolateControlPoints();
    float currentTime = 0.0f;
};

#endif // STEPAHEADANIMATIONCHANNEL_H
