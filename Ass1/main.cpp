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

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL + ImGui", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    setupImGui(window);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        renderImGui();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    cleanupImGui();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}