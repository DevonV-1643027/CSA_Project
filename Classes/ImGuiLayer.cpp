#include "../Headers/ImGuiLayer.h"
#include "../Headers/Animation.h"
#include "../Headers/BackgroundChannel.h"
#include "../Headers/VirtualCameraChannel.h"
#include "../Headers/StepAheadAnimationChannel.h"
#include "../Headers/CharacterAnimationChannel.h"
#include "../Headers/KeyFrame.h"
#include "../Headers/Channel.h"

#include <memory>
#include <iostream>
#include "../imgui-master/imgui.h"
#include "../imgui-master/backends/imgui_impl_glfw.h"
#include "../imgui-master/backends/imgui_impl_opengl3.h"

// Globals
std::shared_ptr<Channel> selectedChannel;
Animation animation("My Animation");

// Function to convert Euler angles to quaternion
glm::quat eulerToQuaternion(float pitch, float yaw, float roll) {
    return glm::quat(glm::vec3(pitch, yaw, roll));
}

// Function to convert quaternion to Euler angles
glm::vec3 quaternionToEuler(const glm::quat& quat) {
    return glm::eulerAngles(quat);
}

void setupImGui(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
}

void renderBackgroundEditor() {
    ImGui::Begin("Background Channel Editor");

    static char texturePath[256] = "";
    ImGui::InputText("Texture Path", texturePath, IM_ARRAYSIZE(texturePath));

    if (ImGui::Button("Load Texture")) {
        if (selectedChannel && selectedChannel->getType() == BACKGROUND) {
            std::dynamic_pointer_cast<BackgroundChannel>(selectedChannel)->loadTexture(texturePath);
            std::fill(std::begin(texturePath), std::end(texturePath), 0);
        }
    }

    if (ImGui::Button("Close Background Editor")) {
        ImGui::CloseCurrentPopup();
    }

    ImGui::End();
}


void renderKeyFrameEditor() {
    ImGui::Begin("Key-Frame Editor");

    static float timestamp = 0.0f;
    static float position[3] = { 0.0f, 0.0f, 0.0f };
    static float rotation[3] = { 0.0f, 0.0f, 0.0f }; // Euler angles
    static float scale[3] = { 1.0f, 1.0f, 1.0f };

    ImGui::InputFloat("Time", &timestamp);
    ImGui::InputFloat3("Position", position);
    ImGui::InputFloat3("Rotation", rotation); // Input Euler angles
    ImGui::InputFloat3("Scale", scale);

    if (ImGui::Button("Close Key-Frame Editor")) {
        ImGui::CloseCurrentPopup();
    }

    if (ImGui::Button("Add Key-Frame")) {
        // Convert Euler angles to quaternion
        glm::quat rotQuat = eulerToQuaternion(rotation[0], rotation[1], rotation[2]);

        // Add new key-frame logic
        KeyFrame newKeyFrame(timestamp, glm::vec3(position[0], position[1], position[2]),
            rotQuat,
            glm::vec3(scale[0], scale[1], scale[2]));
        if (selectedChannel) {
            selectedChannel->addKeyFrame(newKeyFrame);
            // Clear input fields
            timestamp = 0.0f;
            position[0] = position[1] = position[2] = 0.0f;
            rotation[0] = rotation[1] = rotation[2] = 0.0f;
            scale[0] = scale[1] = scale[2] = 1.0f;
        }
    }

    ImGui::Separator();

    ImGui::Text("Existing Key-Frames");
    static int selectedKeyFrameIndex = -1;
    if (selectedChannel) {
        if (ImGui::BeginListBox("##keyframeList")) {
            for (int i = 0; i < selectedChannel->getKeyFrames().size(); ++i) {
                const auto& kf = selectedChannel->getKeyFrames()[i];
                glm::vec3 eulerRot = quaternionToEuler(kf.rotation);
                std::string keyframeLabel = "Time: " + std::to_string(kf.timestamp) +
                    ", Position: (" + std::to_string(kf.position.x) + ", " + std::to_string(kf.position.y) + ", " + std::to_string(kf.position.z) + ")" +
                    ", Rotation: (" + std::to_string(eulerRot.x) + ", " + std::to_string(eulerRot.y) + ", " + std::to_string(eulerRot.z) + ")" +
                    ", Scale: (" + std::to_string(kf.scale.x) + ", " + std::to_string(kf.scale.y) + ", " + std::to_string(kf.scale.z) + ")";
                if (ImGui::Selectable(keyframeLabel.c_str(), selectedKeyFrameIndex == i)) {
                    selectedKeyFrameIndex = i;
                }
            }
            ImGui::EndListBox();
        }

        if (selectedKeyFrameIndex != -1) {
            if (ImGui::Button("Update Key-Frame")) {
                glm::quat rotQuat = eulerToQuaternion(rotation[0], rotation[1], rotation[2]);
                KeyFrame updatedKeyFrame(timestamp, glm::vec3(position[0], position[1], position[2]), rotQuat, glm::vec3(scale[0], scale[1], scale[2]));
                selectedChannel->updateKeyFrame(selectedKeyFrameIndex, updatedKeyFrame);
            }

            if (ImGui::Button("Remove Key-Frame")) {
                selectedChannel->removeKeyFrame(selectedKeyFrameIndex);
                selectedKeyFrameIndex = -1; // Clear selection
            }

            if (ImGui::Button("Move Up") && selectedKeyFrameIndex > 0) {
                selectedChannel->swapKeyFrames(selectedKeyFrameIndex, selectedKeyFrameIndex - 1);
                --selectedKeyFrameIndex;
            }

            if (ImGui::Button("Move Down") && selectedKeyFrameIndex < selectedChannel->getKeyFrames().size() - 1) {
                selectedChannel->swapKeyFrames(selectedKeyFrameIndex, selectedKeyFrameIndex + 1);
                ++selectedKeyFrameIndex;
            }
        }
    }

    ImGui::End();
}



void renderChannelManager() {
    ImGui::Begin("Channel Manager");

    static char channelName[128] = "";
    static int channelType = 0; // 0: Background, 1: VirtualCamera, 2: StepAheadAnimation, 3: CharacterAnimation
    static bool showKeyFrameEditor = false;
    static bool showBackgroundEditor = false;

    ImGui::InputText("Channel Name##new", channelName, IM_ARRAYSIZE(channelName));
    ImGui::Combo("Channel Type", &channelType, "Background\0Virtual Camera\0Step Ahead Animation\0Character Animation\0");

    if (ImGui::Button("Add Channel")) {
        std::shared_ptr<Channel> newChannel;
        switch (channelType) {
        case 0: newChannel = std::make_shared<BackgroundChannel>(channelName); break;
        case 1: newChannel = std::make_shared<VirtualCameraChannel>(channelName); break;
        case 2: newChannel = std::make_shared<StepAheadAnimationChannel>(channelName); break;
        case 3: newChannel = std::make_shared<CharacterAnimationChannel>(channelName); break;
        }
        animation.addChannel(newChannel);
        std::fill(std::begin(channelName), std::end(channelName), 0);
    }

    // Button to trigger the rendering of channels
    if (ImGui::Button("Render Channels")) {
        animation.render();
    }

    ImGui::Separator();

    static int selectedChannelIndex = -1; // Index for the selected channel
    if (ImGui::BeginListBox("Channels")) {
        for (int i = 0; i < animation.getChannels().size(); ++i) {
            bool isSelected = (selectedChannelIndex == i);
            std::string channelDisplayName = animation.getChannels()[i]->getName() + " (" + animation.getChannels()[i]->getTypeString() + ")";
            if (ImGui::Selectable(channelDisplayName.c_str(), isSelected)) {
                selectedChannelIndex = i;
                selectedChannel = animation.getChannels()[i];
            }
        }
        ImGui::EndListBox();
    }

    // Buttons for moving channels up and down
    if (selectedChannelIndex > 0 && ImGui::Button("Move Up")) {
        animation.swapChannels(selectedChannelIndex, selectedChannelIndex - 1);
        --selectedChannelIndex;
    }
    if (selectedChannelIndex < animation.getChannels().size() - 1 && ImGui::Button("Move Down")) {
        animation.swapChannels(selectedChannelIndex, selectedChannelIndex + 1);
        ++selectedChannelIndex;
    }

    if (selectedChannel) {
        static char newChannelName[128] = "";
        ImGui::InputText("New Channel Name##edit", newChannelName, IM_ARRAYSIZE(newChannelName));

        if (ImGui::Button("Change Channel Name")) {
            animation.updateChannelName(selectedChannel->getName(), newChannelName);
            std::fill(std::begin(newChannelName), std::end(newChannelName), 0);
        }

        if (ImGui::Button("Remove Selected Channel")) {
            animation.removeChannel(selectedChannel->getName());
            selectedChannel.reset();
            selectedChannelIndex = -1; // Reset the selected index
        }

        if (ImGui::Button("Edit Channel")) {
            if (selectedChannel->getType() == BACKGROUND) {
				showBackgroundEditor = true;
                ImGui::OpenPopup(("Background Channel Editor##" + std::to_string(selectedChannelIndex)).c_str());
			}
			else {
				showKeyFrameEditor = true;
                ImGui::OpenPopup(("Key-Frame Editor##" + std::to_string(selectedChannelIndex)).c_str());
			}
        }

        if (ImGui::BeginPopupModal(("Background Channel Editor##" + std::to_string(selectedChannelIndex)).c_str(), &showBackgroundEditor, ImGuiWindowFlags_AlwaysAutoResize)) {
            renderBackgroundEditor();
            if (ImGui::Button("Close")) {
                showBackgroundEditor = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if (ImGui::BeginPopupModal(("Key-Frame Editor##" + std::to_string(selectedChannelIndex)).c_str(), &showKeyFrameEditor, ImGuiWindowFlags_AlwaysAutoResize)) {
            renderKeyFrameEditor();
            if (ImGui::Button("Close")) {
                showKeyFrameEditor = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    ImGui::End();
}


void renderImGui() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    renderChannelManager();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void cleanupImGui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
