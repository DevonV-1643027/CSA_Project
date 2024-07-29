#include <GL/glew.h>
#include "../Headers/BackgroundChannel.h"
#include "../Headers/Shader.h"
#include <stb_image.h> // For loading images
#include <iostream>

// Function to initialize GLEW
bool initializeGLEW() {
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
        return false;
    }
    std::cout << "GLEW initialized successfully" << std::endl;
    return true;
}

BackgroundChannel::BackgroundChannel(const std::string& name) : Channel(name, BACKGROUND), setupCompleted(false), backgroundShader(nullptr) {
    // Initialization is deferred to the setupBackground method
}

BackgroundChannel::~BackgroundChannel() {
    glDeleteVertexArrays(1, &backgroundVAO);
    glDeleteBuffers(1, &backgroundVBO);
    delete backgroundShader;
}

void BackgroundChannel::loadTexture(const std::string& texturePath) {
    if (!setupCompleted) {
        setupBackground();
    }

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
        std::cout << "Loaded texture: " << texturePath << ", Width: " << width << ", Height: " << height << ", Channels: " << nrChannels << std::endl;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

        // Check for errors before generating mipmaps
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cerr << "OpenGL error before generating mipmaps: " << error << std::endl;
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        // Check for errors after generating mipmaps
        error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cerr << "OpenGL error after generating mipmaps: " << error << std::endl;
        }

        stbi_image_free(data);
    }
    else {
        std::cout << "Failed to load texture: " << texturePath << std::endl;
        stbi_image_free(data);
    }
}

void BackgroundChannel::update(float deltaTime) {
    // No update logic for background channel
}

void BackgroundChannel::render() {
    std::cout << "Rendering background channel" << std::endl;

    if (!setupCompleted) {
        setupBackground();
        if (!setupCompleted) {
            std::cerr << "Background setup not completed. Aborting render." << std::endl;
            return;
        }
    }

    // Use the shader program
    glUseProgram(backgroundShader->ID);

    // Bind the vertex array object
    glBindVertexArray(backgroundVAO);

    // Activate the texture unit first before binding texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set the texture uniform in the shader (usually required)
    GLint textureUniformLocation = glGetUniformLocation(backgroundShader->ID, "ourTexture");
    if (textureUniformLocation != -1) {
        glUniform1i(textureUniformLocation, 0);
    }
    else {
        std::cerr << "Could not find texture uniform location." << std::endl;
    }

    // Clear the screen before drawing (optional, depending on your application's needs)
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the background
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Check for OpenGL errors
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error during rendering: " << error << std::endl;
    }

    // Unbind the vertex array object and shader program (clean up state)
    glBindVertexArray(0);
    glUseProgram(0);
}


void BackgroundChannel::setupBackground() {
    if (!initializeGLEW()) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return;
    }

    float vertices[] = {
        // positions          // texture coords
        1.0f,  1.0f, 0.0f,   1.0f, 1.0f,
        1.0f, -1.0f, 0.0f,   1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
        1.0f,  1.0f, 0.0f,   1.0f, 1.0f
    };

    glGenVertexArrays(1, &backgroundVAO);
    glGenBuffers(1, &backgroundVBO);
    glBindVertexArray(backgroundVAO);

    glBindBuffer(GL_ARRAY_BUFFER, backgroundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    backgroundShader = new Shader("../Shaders/background.vs", "../Shaders/background.fs");

    setupCompleted = true;
}
