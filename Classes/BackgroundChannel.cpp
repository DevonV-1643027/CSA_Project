#include <GL/glew.h> // Ensure this is included before other OpenGL headers
#include <GL/gl.h>   // OpenGL headers
#include "../Headers/BackgroundChannel.h"
#include "../Headers/Shader.h"
#include <stb_image.h> // For loading images
#include <iostream>
#include <string> // Include this if you are using std::string

BackgroundChannel::BackgroundChannel(const std::string& name)
    : Channel(name, BACKGROUND), backgroundVAO(0), backgroundVBO(0), textureID(0), backgroundShader(nullptr), setupCompleted(false) {
}

BackgroundChannel::~BackgroundChannel() {
    glDeleteVertexArrays(1, &backgroundVAO);
    glDeleteBuffers(1, &backgroundVBO);
    glDeleteTextures(1, &textureID);
    delete backgroundShader;
}

void BackgroundChannel::setupBackground() {
    // Background quad vertices
    float backgroundVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
    };

    // Setup VAO and VBO
    glGenVertexArrays(1, &backgroundVAO);
    glGenBuffers(1, &backgroundVBO);
    glBindVertexArray(backgroundVAO);

    glBindBuffer(GL_ARRAY_BUFFER, backgroundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundVertices), backgroundVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);

    // Load and compile shader
    backgroundShader = new Shader("background.vert", "background.frag");
    setupCompleted = true;
}

void BackgroundChannel::loadTexture(const std::string& texturePath) {
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load image
    int width, height, nrChannels;
    unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}

void BackgroundChannel::update(float deltaTime) {
    // No update logic for background channel yet
}

void BackgroundChannel::render() {
    if (!setupCompleted) {
        setupBackground();
    }

    backgroundShader->use();
    glBindVertexArray(backgroundVAO);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
