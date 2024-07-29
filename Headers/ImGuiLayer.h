#ifndef IMGUI_LAYER_H
#define IMGUI_LAYER_H

#include <memory>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

// Forward declare the Channel class to avoid circular dependency
class Channel;

void setupImGui(GLFWwindow* window);
void renderImGui();
void cleanupImGui();

void renderKeyFrameEditor(); // New function to render the key-frame editor

extern std::shared_ptr<Channel> selectedChannel; // Declare selectedChannel as extern

#endif // IMGUI_LAYER_H
