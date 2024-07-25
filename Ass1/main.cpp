#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include "../Headers/Channel.h"
#include "../Headers/BackgroundChannel.h"
#include "../Headers/VirtualCameraChannel.h"
#include "../Headers/StepAheadAnimationChannel.h"
#include "../Headers/CharacterAnimationChannel.h"
#include "../Headers/KeyFrame.h"
#include "../Headers/Animation.h"
#include "../Headers/ImGuiLayer.h"

void printChannelKeyFrames(const Channel& channel) {
    std::cout << channel.getName() << " KeyFrames:" << std::endl;
    for (const auto& kf : channel.getKeyFrames()) {
        std::cout << "  KeyFrame - Time: " << kf.timestamp
            << ", Position: (" << kf.position.x << ", " << kf.position.y << ", " << kf.position.z << ")"
            << ", Rotation: (" << kf.rotation.x << ", " << kf.rotation.y << ", " << kf.rotation.z << ")"
            << ", Scale: (" << kf.scale.x << ", " << kf.scale.y << ", " << kf.scale.z << ")" << std::endl;
    }
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a GLFW window
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL + ImGui", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        });

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Setup ImGui
    setupImGui(window);

    // Create an animation
    Animation animation("Test Animation");

    // Instantiate the channels
    auto backgroundChannel = std::make_shared<BackgroundChannel>("Background");
    auto virtualCameraChannel = std::make_shared<VirtualCameraChannel>("VirtualCamera");
    auto stepAheadAnimationChannel = std::make_shared<StepAheadAnimationChannel>("StepAheadAnimation");
    auto characterAnimationChannel = std::make_shared<CharacterAnimationChannel>("CharacterAnimation");

    // Create key frames
    KeyFrame keyFrame1(0.0f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    KeyFrame keyFrame2(1.0f, glm::vec3(1.0f, 2.0f, 3.0f), glm::vec3(10.0f, 20.0f, 30.0f), glm::vec3(1.5f, 1.5f, 1.5f));

    // Add key frames to channels
    backgroundChannel->addKeyFrame(keyFrame1);
    virtualCameraChannel->addKeyFrame(keyFrame2);
    stepAheadAnimationChannel->addKeyFrame(keyFrame1);
    characterAnimationChannel->addKeyFrame(keyFrame2);

    // Add channels to animation
    animation.addChannel(backgroundChannel);
    animation.addChannel(virtualCameraChannel);
    animation.addChannel(stepAheadAnimationChannel);
    animation.addChannel(characterAnimationChannel);

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        // Input
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        // Update and render animation
        animation.update(0.1f);
        glClear(GL_COLOR_BUFFER_BIT);
        animation.render();

        // Render ImGui
        renderImGui();

        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup ImGui
    cleanupImGui();

    // Terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
