#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class WindowManager {
public:
    static WindowManager& getInstance();
    void createWindow(int width, int height, const char* title);
    void setFramebufferSizeCallback(GLFWframebuffersizefun callback);
    GLFWwindow* getWindow() const;

private:
    WindowManager() : window(nullptr) {}
    WindowManager(const WindowManager&) = delete;
    WindowManager& operator=(const WindowManager&) = delete;

    GLFWwindow* window;
};