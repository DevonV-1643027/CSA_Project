#include "../Headers/Camera.h"

// Private constructor to prevent instantiation
Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : Position(position), WorldUp(up), Yaw(yaw), Pitch(pitch),
    Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(2.5f), MouseSensitivity(0.1f),
    Zoom(45.0f), RotationSpeed(50.0f) {
    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(Position, Position + Front, Up);
}

void Camera::ProcessKeyboard(GLFWwindow* window, float deltaTime) {
    float velocity = MovementSpeed * deltaTime;
    float rotationVelocity = RotationSpeed * deltaTime;

    // Handle movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        Position += Front * velocity;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        Position -= Front * velocity;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        Position -= Right * velocity;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        Position += Right * velocity;

    // Handle rotation
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        Yaw -= rotationVelocity;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        Yaw += rotationVelocity;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        Pitch += rotationVelocity;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        Pitch -= rotationVelocity;

    if (Pitch > 89.0f)
        Pitch = 89.0f;
    if (Pitch < -89.0f)
        Pitch = -89.0f;

    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}
