#pragma once
#ifndef BACKGROUND_CHANNEL_H
#define BACKGROUND_CHANNEL_H

#include <string>
#include "Channel.h"
#include "Shader.h"

class BackgroundChannel : public Channel {
public:
    BackgroundChannel(const std::string& name);
    ~BackgroundChannel();

    void loadTexture(const std::string& texturePath);
    virtual void update(float deltaTime) override;
    virtual void render() override;

private:
    void setupBackground();

    GLuint backgroundVAO, backgroundVBO, textureID;
    Shader* backgroundShader; // Add this line to declare backgroundShader
    bool setupCompleted;  // Flag to check if setup is done
};

#endif // BACKGROUND_CHANNEL_H
