#include "../Headers/BackgroundChannel.h"
#include "../Headers/Shader.h"
#include <stb_image.h> // For loading images
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

// Function to initialize GLEW
bool initializeGLEW() {
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
        return false;
    }
    return true;
}

BackgroundChannel::BackgroundChannel(const std::string& name)
    : Channel(name, BACKGROUND), setupCompleted(false), backgroundShader(nullptr) {
    // Initialization is deferred to the setupBackground method
}

BackgroundChannel::~BackgroundChannel() {
    if (backgroundVAO != 0) {
        glDeleteVertexArrays(1, &backgroundVAO);
        backgroundVAO = 0; // Avoid dangling reference
    }
    if (backgroundVBO != 0) {
        glDeleteBuffers(1, &backgroundVBO);
        backgroundVBO = 0; // Avoid dangling reference
    }
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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }
    else {
        std::cerr << "Failed to load texture: " << texturePath << std::endl;
        stbi_image_free(data);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

void BackgroundChannel::loadSkybox(const std::vector<std::string>& faces) {
    if (!setupCompleted) {
        setupBackground();
    }

    glGenTextures(1, &skyboxTextureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else {
            std::cerr << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void BackgroundChannel::setupBackground() {
    if (!initializeGLEW()) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return;
    }

    // Check if the shaders are compiled and linked correctly
    backgroundShader = new Shader("../Shaders/background.vs", "../Shaders/background.fs");
    if (!backgroundShader->isCompiled()) {
        std::cerr << "Failed to compile and link shader" << std::endl;
        return;
    }
    
    float vertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &backgroundVAO);
    glGenBuffers(1, &backgroundVBO);
    glBindVertexArray(backgroundVAO);
    glBindBuffer(GL_ARRAY_BUFFER, backgroundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    setupCompleted = true;
}

void BackgroundChannel::render(const glm::mat4& view, const glm::mat4& projection) {
    if (!setupCompleted) {
        std::cerr << "Background setup not completed. Aborting render." << std::endl;
        return;
    }

    glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content

    glUseProgram(backgroundShader->ID); // Ensure the correct shader program is being used

    // Bind the vertex array object
    glBindVertexArray(backgroundVAO);

    // Set view and projection matrices
    glUniformMatrix4fv(glGetUniformLocation(backgroundShader->ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(backgroundShader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // Activate the texture unit first before binding texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);

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
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Unbind the vertex array object and shader program (clean up state)
    glBindVertexArray(0);
    glUseProgram(0);
    glDepthFunc(GL_LESS); // Set depth function back to default
}

void BackgroundChannel::update(float deltaTime) {
    // No update logic for background channel
}