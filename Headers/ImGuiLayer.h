#pragma once
#ifndef IMGUI_LAYER_H
#define IMGUI_LAYER_H

#include <memory>
#include "../Headers/Animation.h"


// Forward declare the Channel class to avoid circular dependency
class Channel;

void setupImGui(GLFWwindow* window, Animation* aMAIN);
void renderImGui();
void cleanupImGui();

void renderKeyFrameEditor(); // New function to render the key-frame editor

extern std::shared_ptr<Channel> selectedChannel; // Declare selectedChannel as extern

#endif // IMGUI_LAYER_H