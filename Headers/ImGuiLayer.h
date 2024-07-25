#pragma once
#ifndef IMGUI_LAYER_H
#define IMGUI_LAYER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include "../imgui-master/imgui.h"
#include "../imgui-master/backends/imgui_impl_glfw.h"
#include "../imgui-master/backends/imgui_impl_opengl3.h"
void setupImGui(GLFWwindow* window);
void renderImGui();
void cleanupImGui();

#endif // IMGUI_LAYER_H
