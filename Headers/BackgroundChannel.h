#pragma once
#ifndef BACKGROUND_CHANNEL_H
#define BACKGROUND_CHANNEL_H

#include <string>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Channel.h"
#include "ShaderD.h"
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h> // For loading images
#include <iostream>

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
    ShaderD* backgroundShader;
    bool setupCompleted;
};

#endif // BACKGROUND_CHANNEL_H
