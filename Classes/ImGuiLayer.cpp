#include "../Headers/BackgroundChannel.h"
#include "../Headers/ImGuiLayer.h"

#include "../Headers/VirtualCameraChannel.h"
#include "../Headers/StepAheadAnimationChannel.h"
#include "../Headers/CharacterAnimationChannel.h"
#include "../Headers/KeyFrame.h"
#include "../Headers/Channel.h"

#include <iostream>

// Globals
std::shared_ptr<Channel> selectedChannel;
Animation animationGUI("GUI Animation");
Animation* animationMAIN = nullptr;


// Helper functions
// Function to convert Euler angles to quaternion
glm::quat eulerToQuaternion(float pitch, float yaw, float roll) {
    return glm::quat(glm::vec3(pitch, yaw, roll));
}

// Function to convert quaternion to Euler angles
glm::vec3 quaternionToEuler(const glm::quat& quat) {
    return glm::eulerAngles(quat);
}

// Function to get the cubemap faces from a directory
std::vector<std::string> getCubemapFaces(const std::string& directoryPath) {
    std::vector<std::string> faces(6);
    faces[0] = directoryPath + "/right.jpg";
    faces[1] = directoryPath + "/left.jpg";
    faces[2] = directoryPath + "/top.jpg";
    faces[3] = directoryPath + "/bottom.jpg";
    faces[4] = directoryPath + "/front.jpg";
    faces[5] = directoryPath + "/back.jpg";

    return faces;
}

// Function to format float to a fixed number of decimal places
std::string formatFloat(float value, int decimalPlaces) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(decimalPlaces) << value;
    return stream.str();
}

// ImGui functions
void setupImGui(GLFWwindow* window, Animation* aMAIN) {
    animationMAIN = aMAIN;
    animationGUI = *aMAIN; // Make a copy of the Animation instance

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

    static char skyboxPath[256] = "";
    ImGui::InputText("Skybox Directory Path", skyboxPath, IM_ARRAYSIZE(skyboxPath));

    if (ImGui::Button("Load Skybox")) {
        if (selectedChannel && selectedChannel->getType() == BACKGROUND) {
            std::vector<std::string> faces = getCubemapFaces(skyboxPath);
            std::dynamic_pointer_cast<BackgroundChannel>(selectedChannel)->loadSkybox(faces);
            std::fill(std::begin(skyboxPath), std::end(skyboxPath), 0);
        }
    }

    if (ImGui::Button("Close Background Editor")) {
        ImGui::CloseCurrentPopup();
    }

    ImGui::End();
}

void renderStepAheadEditor() {
    ImGui::Begin("Key-Frame Editor");

    static char objFilePath[256] = ""; // Path to OBJ file
    static bool importSuccess = false; // To store the result of import
    static char importStatus[64] = ""; // To display the import status message

    ImGui::InputText("OBJ File Path", objFilePath, IM_ARRAYSIZE(objFilePath));

    if (ImGui::Button("Import OBJ")) {
        if (selectedChannel && selectedChannel->getType() == STEP_AHEAD_ANIMATION) {
            std::dynamic_pointer_cast<StepAheadAnimationChannel>(selectedChannel)->importObject(objFilePath);
            std::fill(std::begin(objFilePath), std::end(objFilePath), 0);
        }
    }

    // Display import status message
    if (importSuccess) {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), importStatus); // Green for success
    }
    else if (strlen(importStatus) > 0) {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), importStatus); // Red for failure
    }

    ImGui::Separator();

    static char vertexShaderPath[256] = ""; // Path to Vertex shader file
    static char fragmentShaderPath[256] = ""; // Path to Fragment shader file

    ImGui::InputText("Vertex Shader Path", vertexShaderPath, IM_ARRAYSIZE(vertexShaderPath));
    ImGui::InputText("Fragment Shader Path", fragmentShaderPath, IM_ARRAYSIZE(fragmentShaderPath));

    if (ImGui::Button("Load Shader")) {
        if (selectedChannel && selectedChannel->getType() == STEP_AHEAD_ANIMATION)
        {
            std::dynamic_pointer_cast<StepAheadAnimationChannel>(selectedChannel)->setupShader(vertexShaderPath, fragmentShaderPath);
            std::fill(std::begin(vertexShaderPath), std::end(vertexShaderPath), 0);
            std::fill(std::begin(fragmentShaderPath), std::end(fragmentShaderPath), 0);
        }   
    }

    ImGui::Separator();

    static float timestamp = 0.0f;
    static float position[3] = { 0.0f, 0.0f, 0.0f };
    static float rotation[3] = { 0.0f, 0.0f, 0.0f }; // Euler angles
    static float scale[3] = { 1.0f, 1.0f, 1.0f };
    static int frameRate = 10; // Default frame rate

    ImGui::InputFloat("Time", &timestamp);
    ImGui::InputFloat3("Position", position);
    ImGui::InputFloat3("Rotation", rotation); // Input Euler angles
    ImGui::InputFloat3("Scale", scale);
    ImGui::InputInt("Frame Rate", &frameRate); // Input frame rate

    ImGui::Separator();

    if (ImGui::Button("Close Step-Ahead Editor")) {
        ImGui::CloseCurrentPopup();
    }

    if (ImGui::Button("Set Frame Rate")) {
        if (selectedChannel) {
            selectedChannel->setFrameRate(frameRate);
        }
    }

    if (ImGui::Button("Add Key-Frame")) {
        // Convert Euler angles to quaternion
        glm::quat rotQuat = eulerToQuaternion(glm::radians(rotation[0]), glm::radians(rotation[1]), glm::radians(rotation[2]));

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
    static int selectedControlPointIndex = -1;
    static float ffdPosition[3] = { 0.0f, 0.0f, 0.0f };
    static float ffdOriginalPosition[3] = { 0.0f, 0.0f, 0.0f };
    static float ffdWeight = 1.0f;

    if (selectedChannel) {
        if (ImGui::BeginListBox("##keyframeList", ImVec2(-FLT_MIN, 10 * ImGui::GetTextLineHeightWithSpacing()))) {
            for (int i = 0; i < selectedChannel->getKeyFrames().size(); ++i) {
                const auto& kf = selectedChannel->getKeyFrames()[i];
                glm::vec3 eulerRot = glm::degrees(quaternionToEuler(kf.rotation)); // Convert from radians to degrees

                // Apply rounding
                std::string keyframeLabel = "Time: " + formatFloat(kf.timestamp, 2) +
                    ", Position: (" + formatFloat(kf.position.x, 2) + ", " + formatFloat(kf.position.y, 2) + ", " + formatFloat(kf.position.z, 2) + ")" +
                    ", Rotation: (" + formatFloat(eulerRot.x, 1) + ", " + formatFloat(eulerRot.y, 1) + ", " + formatFloat(eulerRot.z, 1) + ")" +
                    ", Scale: (" + formatFloat(kf.scale.x, 2) + ", " + formatFloat(kf.scale.y, 2) + ", " + formatFloat(kf.scale.z, 2) + ")";
                if (ImGui::Selectable(keyframeLabel.c_str(), selectedKeyFrameIndex == i)) {
                    selectedKeyFrameIndex = i;
                }
            }
            ImGui::EndListBox();
        }

        if (selectedKeyFrameIndex != -1) {
            KeyFrame& kf = selectedChannel->getKeyFrames()[selectedKeyFrameIndex];

            timestamp = kf.timestamp;
            position[0] = kf.position.x;
            position[1] = kf.position.y;
            position[2] = kf.position.z;
            glm::vec3 eulerRot = glm::degrees(quaternionToEuler(kf.rotation)); // Convert from radians to degrees
            rotation[0] = eulerRot.x;
            rotation[1] = eulerRot.y;
            rotation[2] = eulerRot.z;
            scale[0] = kf.scale.x;
            scale[1] = kf.scale.y;
            scale[2] = kf.scale.z;

            if (ImGui::Button("Update Key-Frame")) {
                glm::quat rotQuat = eulerToQuaternion(glm::radians(rotation[0]), glm::radians(rotation[1]), glm::radians(rotation[2]));
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

            ImGui::Separator();
            ImGui::Text("FFD Control Points");

            if (ImGui::BeginListBox("##controlPointList", ImVec2(-FLT_MIN, 10 * ImGui::GetTextLineHeightWithSpacing()))) {
                for (int i = 0; i < kf.ffdControlPoints.size(); ++i) {
                    const auto& cp = kf.ffdControlPoints[i];
                    std::string controlPointLabel = "Control Point " + std::to_string(i) +
                        ": Pos(" + std::to_string(cp.position.x) + ", " + std::to_string(cp.position.y) + ", " + std::to_string(cp.position.z) + ")" +
                        ", OrigPos(" + std::to_string(cp.originalPosition.x) + ", " + std::to_string(cp.originalPosition.y) + ", " + std::to_string(cp.originalPosition.z) + ")" +
                        ", Weight(" + std::to_string(cp.weight) + ")";
                    if (ImGui::Selectable(controlPointLabel.c_str(), selectedControlPointIndex == i)) {
                        selectedControlPointIndex = i;
                        ffdPosition[0] = cp.position.x;
                        ffdPosition[1] = cp.position.y;
                        ffdPosition[2] = cp.position.z;
                        ffdWeight = cp.weight;
                    }
                }
                ImGui::EndListBox();
            }

            if (selectedControlPointIndex != -1) {
                if (ImGui::Button("Remove Control Point")) {
                    kf.ffdControlPoints.erase(kf.ffdControlPoints.begin() + selectedControlPointIndex);
                    selectedControlPointIndex = -1; // Clear selection
                }
            }

            ImGui::Separator();
            ImGui::Text("Add New FFD Control Point");
            ImGui::InputFloat3("Position", ffdPosition);
            ImGui::InputFloat3("Original Position", ffdOriginalPosition);
            ImGui::InputFloat("Weight", &ffdWeight);

            if (ImGui::Button("Add Control Point")) {
                kf.ffdControlPoints.emplace_back(
                    glm::vec3(ffdPosition[0], ffdPosition[1], ffdPosition[2]),
                    glm::vec3(ffdOriginalPosition[0], ffdOriginalPosition[1], ffdOriginalPosition[2]),
                    ffdWeight
                );
                selectedControlPointIndex = kf.ffdControlPoints.size() - 1; // Select the new control point
            }
        }
    }

    ImGui::End();
}


void renderKeyFrameEditor() {
    ImGui::Begin("Key-Frame Editor");

    static float timestamp = 0.0f;
    static float position[3] = { 0.0f, 0.0f, 0.0f };
    static float rotation[3] = { 0.0f, 0.0f, 0.0f }; // Euler angles
    static float scale[3] = { 1.0f, 1.0f, 1.0f };
    static int frameRate = 10; // Default frame rate

    ImGui::InputFloat("Time", &timestamp);
    ImGui::InputFloat3("Position", position);
    ImGui::InputFloat3("Rotation", rotation); // Input Euler angles
    ImGui::InputFloat3("Scale", scale);
    ImGui::InputInt("Frame Rate", &frameRate); // Input frame rate

    ImGui::Separator();

    if (ImGui::Button("Close Key-Frame Editor")) {
        ImGui::CloseCurrentPopup();
    }
    
    if (ImGui::Button("Set Frame Rate")) {
		if (selectedChannel) {
			selectedChannel->setFrameRate(frameRate);
		}
	}

    if (ImGui::Button("Add Key-Frame")) {
        // Convert Euler angles to quaternion
        glm::quat rotQuat = eulerToQuaternion(glm::radians(rotation[0]), glm::radians(rotation[1]), glm::radians(rotation[2]));

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
        if (ImGui::BeginListBox("##keyframeList", ImVec2(-FLT_MIN, 10 * ImGui::GetTextLineHeightWithSpacing()))) {
            for (int i = 0; i < selectedChannel->getKeyFrames().size(); ++i) {
                const auto& kf = selectedChannel->getKeyFrames()[i];
                glm::vec3 eulerRot = glm::degrees(quaternionToEuler(kf.rotation)); // Convert from radians to degrees

                // Apply rounding
                std::string keyframeLabel = "Time: " + formatFloat(kf.timestamp, 2) +
                    ", Position: (" + formatFloat(kf.position.x, 2) + ", " + formatFloat(kf.position.y, 2) + ", " + formatFloat(kf.position.z, 2) + ")" +
                    ", Rotation: (" + formatFloat(eulerRot.x, 1) + ", " + formatFloat(eulerRot.y, 1) + ", " + formatFloat(eulerRot.z, 1) + ")" +
                    ", Scale: (" + formatFloat(kf.scale.x, 2) + ", " + formatFloat(kf.scale.y, 2) + ", " + formatFloat(kf.scale.z, 2) + ")";
                if (ImGui::Selectable(keyframeLabel.c_str(), selectedKeyFrameIndex == i)) {
                    selectedKeyFrameIndex = i;
                }
            }
            ImGui::EndListBox();
        }

        if (selectedKeyFrameIndex != -1) {
            const auto& kf = selectedChannel->getKeyFrames()[selectedKeyFrameIndex];
            timestamp = kf.timestamp;
            position[0] = kf.position.x;
            position[1] = kf.position.y;
            position[2] = kf.position.z;
            glm::vec3 eulerRot = glm::degrees(quaternionToEuler(kf.rotation)); // Convert from radians to degrees
            rotation[0] = eulerRot.x;
            rotation[1] = eulerRot.y;
            rotation[2] = eulerRot.z;
            scale[0] = kf.scale.x;
            scale[1] = kf.scale.y;
            scale[2] = kf.scale.z;

            if (ImGui::Button("Update Key-Frame")) {
                glm::quat rotQuat = eulerToQuaternion(glm::radians(rotation[0]), glm::radians(rotation[1]), glm::radians(rotation[2]));
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
    static bool showStepAheadEditor = false;

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
        animationGUI.addChannel(newChannel);
        std::fill(std::begin(channelName), std::end(channelName), 0);
    }

    // Button to trigger the rendering of channels
    if (ImGui::Button("Render Channels")) {
        *animationMAIN = animationGUI; // Copy the GUI animation to the main animation
    }

    ImGui::Separator();

    static int selectedChannelIndex = -1; // Index for the selected channel
    if (ImGui::BeginListBox("Channels")) {
        for (int i = 0; i < animationGUI.getChannels().size(); ++i) {
            bool isSelected = (selectedChannelIndex == i);
            std::string channelDisplayName = animationGUI.getChannels()[i]->getName() + " (" + animationGUI.getChannels()[i]->getTypeString() + ")";
            if (ImGui::Selectable(channelDisplayName.c_str(), isSelected)) {
                selectedChannelIndex = i;
                selectedChannel = animationGUI.getChannels()[i];
            }
        }
        ImGui::EndListBox();
    }

    // Buttons for moving channels up and down
    if (selectedChannelIndex > 0 && ImGui::Button("Move Up")) {
        animationGUI.swapChannels(selectedChannelIndex, selectedChannelIndex - 1);
        --selectedChannelIndex;
    }
    if (selectedChannelIndex < animationGUI.getChannels().size() - 1 && ImGui::Button("Move Down")) {
        animationGUI.swapChannels(selectedChannelIndex, selectedChannelIndex + 1);
        ++selectedChannelIndex;
    }

    if (selectedChannel) {
        static char newChannelName[128] = "";
        ImGui::InputText("New Channel Name##edit", newChannelName, IM_ARRAYSIZE(newChannelName));

        if (ImGui::Button("Change Channel Name")) {
            animationGUI.updateChannelName(selectedChannel->getName(), newChannelName);
            std::fill(std::begin(newChannelName), std::end(newChannelName), 0);
        }

        if (ImGui::Button("Remove Selected Channel")) {
            animationGUI.removeChannel(selectedChannel->getName());
            selectedChannel.reset();
            selectedChannelIndex = -1; // Reset the selected index
        }

        if (ImGui::Button("Edit Channel")) {
            if (selectedChannel->getType() == BACKGROUND) {
				showBackgroundEditor = true;
                ImGui::OpenPopup(("Background Channel Editor##" + std::to_string(selectedChannelIndex)).c_str());
			}
            else if (selectedChannel->getType() == STEP_AHEAD_ANIMATION) {
				showStepAheadEditor = true;
				ImGui::OpenPopup(("Step Ahead Editor##" + std::to_string(selectedChannelIndex)).c_str());
			}
            else {
				showKeyFrameEditor = true;
                ImGui::OpenPopup(("Key-Frame Editor##" + std::to_string(selectedChannelIndex)).c_str());
			}
        }

        if (selectedChannel->getType() == VIRTUAL_CAMERA) {
            if (ImGui::Button("Render Path")) {
                std::dynamic_pointer_cast<VirtualCameraChannel>(selectedChannel)->startTraversal();
            }
        }

        if (ImGui::Button("Activate")) {
            selectedChannel->isActive = true;
        }

        if (ImGui::Button("Deactivate")) {
            selectedChannel->isActive = false;
        }

        if (ImGui::BeginPopupModal(("Background Channel Editor##" + std::to_string(selectedChannelIndex)).c_str(), &showBackgroundEditor, ImGuiWindowFlags_AlwaysAutoResize)) {
            renderBackgroundEditor();
            if (ImGui::Button("Close")) {
                showBackgroundEditor = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if (ImGui::BeginPopupModal(("Step Ahead Editor##" + std::to_string(selectedChannelIndex)).c_str(), &showStepAheadEditor, ImGuiWindowFlags_AlwaysAutoResize)) {
			renderStepAheadEditor();
			if (ImGui::Button("Close")) {
                showStepAheadEditor = false;
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
