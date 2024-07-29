#include "../Headers/WindowManager.h"
#include <iostream>
#include "../Headers/ImGuiLayer.h"
#include "../Headers/BackgroundChannel.h"
#include "../Headers/Animation.h"
#include "../Headers/Camera.h"  // Include the Camera header
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Define a global or class-level camera object
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window, float deltaTime) {
    camera.processKeyboardInput(window, deltaTime);
}

int main() {
    WindowManager& wm = WindowManager::getInstance();
    try {
        wm.createWindow(1600, 1200, "OpenGL + ImGui");
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    wm.setFramebufferSizeCallback(framebuffer_size_callback);

    Animation animation("Main Animation");

    setupImGui(wm.getWindow(), &animation);

    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(wm.getWindow())) {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(wm.getWindow(), deltaTime);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Update the view matrix in the animation rendering (if applicable)
        // For example, pass the view matrix to shaders
        glm::mat4 view = camera.getViewMatrix();
        // Pass this view matrix to your shaders if needed

        animation.render();
        renderImGui();

        glfwSwapBuffers(wm.getWindow());
        glfwPollEvents();
    }

    cleanupImGui();

    glfwDestroyWindow(wm.getWindow());
    glfwTerminate();
    return 0;
}