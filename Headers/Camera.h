#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

class Camera {
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    float RotationSpeed;

    // Public method to get the instance of the Camera class
    static Camera& getInstance(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = -90.0f,
        float pitch = 0.0f) {
        static Camera instance(position, up, yaw, pitch);
        return instance;
    }

    // Delete copy constructor and assignment operator
    Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;

    glm::mat4 GetViewMatrix() const;
    void ProcessKeyboard(GLFWwindow* window, float deltaTime);

private:
    // Private constructor to prevent instantiation
    Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch);

    void updateCameraVectors();
};

#endif // CAMERA_H
