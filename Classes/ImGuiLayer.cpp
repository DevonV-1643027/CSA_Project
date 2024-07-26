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

    if (ImGui::Button("Add Key-Frame")) {
        // Convert Euler angles to quaternion
        glm::quat rotQuat = eulerToQuaternion(rotation[0], rotation[1], rotation[2]);

        // Debug print to check input values
        std::cout << "Adding KeyFrame: Time=" << timestamp
            << ", Position=(" << position[0] << ", " << position[1] << ", " << position[2] << ")"
            << ", Rotation=(" << rotation[0] << ", " << rotation[1] << ", " << rotation[2] << ")"
            << ", Scale=(" << scale[0] << ", " << scale[1] << ", " << scale[2] << ")" << std::endl;

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
    if (selectedChannel) {
        for (const auto& kf : selectedChannel->getKeyFrames()) {
            // Convert quaternion to Euler angles for display
            glm::vec3 eulerRot = quaternionToEuler(kf.rotation);

            ImGui::Text("Time: %.3f, Position: (%.3f, %.3f, %.3f), Rotation: (%.3f, %.3f, %.3f), Scale: (%.3f, %.3f, %.3f)",
                kf.timestamp, kf.position.x, kf.position.y, kf.position.z,
                eulerRot.x, eulerRot.y, eulerRot.z,
                kf.scale.x, kf.scale.y, kf.scale.z);
        }
    }

    if (ImGui::Button("Close Key-Frame Editor")) {
        ImGui::CloseCurrentPopup();
    }

    ImGui::End();
}

void renderChannelManager() {
    ImGui::Begin("Channel Manager");

    static char channelName[128] = "";
    static int channelType = 0; // 0: Background, 1: VirtualCamera, 2: StepAheadAnimation, 3: CharacterAnimation

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

        if (ImGui::Button("Edit Key-Frames")) {
            // Open the KeyFrame editor for the selected channel
            ImGui::OpenPopup("Key-Frame Editor");
        }

        if (ImGui::BeginPopupModal("Key-Frame Editor", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            renderKeyFrameEditor();
            if (ImGui::Button("Close")) {
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
