#include "../Headers/WindowManager.h"
#include <iostream>
#include <stdexcept>

WindowManager& WindowManager::getInstance() {
    static WindowManager instance;
    return instance;
}

void WindowManager::createWindow(int width, int height, const char* title) {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        throw std::runtime_error("Failed to initialize GLAD");
    }
}

void WindowManager::setFramebufferSizeCallback(GLFWframebuffersizefun callback) {
    if (window) {
        glfwSetFramebufferSizeCallback(window, callback);
    }
}

GLFWwindow* WindowManager::getWindow() const {
    return window;
}

void WindowManager::setWindow(GLFWwindow* window) {
    this->window = window;
}