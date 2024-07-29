#include "../Headers/WindowManager.h"
#include <iostream>
#include "../Headers/ImGuiLayer.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
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

    setupImGui(wm.getWindow());

    while (!glfwWindowShouldClose(wm.getWindow())) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        renderImGui();

        glfwSwapBuffers(wm.getWindow());
        glfwPollEvents();
    }

    cleanupImGui();

    glfwDestroyWindow(wm.getWindow());
    glfwTerminate();
    return 0;
}
