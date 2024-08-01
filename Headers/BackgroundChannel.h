#pragma once
#ifndef BACKGROUND_CHANNEL_H
#define BACKGROUND_CHANNEL_H

#include <string>
#include <vector>
#include <GL/glew.h>
#include "Channel.h"
#include "Shader.h"

class BackgroundChannel : public Channel {
public:
    BackgroundChannel(const std::string& name);
    ~BackgroundChannel();

    void loadTexture(const std::string& texturePath);
    void loadSkybox(const std::vector<std::string>& faces);
    virtual void update(float deltaTime) override;
    virtual void render(const glm::mat4& view, const glm::mat4& projection) override;  // Update render function

private:
    void setupBackground();

    GLuint backgroundVAO = 0;
    GLuint backgroundVBO = 0;
    GLuint textureID = 0;
    GLuint skyboxTextureID;
    Shader* backgroundShader;
    bool setupCompleted;
};

#endif // BACKGROUND_CHANNEL_H
