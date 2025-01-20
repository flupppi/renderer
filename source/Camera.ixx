module;
#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
export module Camera;

namespace Engine {


    export class Camera {
    public:
        enum class CameraMode {
            FPS,
            Orbit,
        };

        Camera(float distance = 10.0f, float angle = 0.0f, float speed = 1.0f, float aspectRatio = 4.0f / 3.0f);

        // Update camera angle and distance based on user input
        void Update(float deltaTime, bool rotateLeft, bool rotateRight, bool zoomIn, bool zoomOut);

        // Get the view matrix based on the current camera position and orientation
        glm::mat4 GetViewMatrix() const;

        // Get the projection matrix
        glm::mat4 GetProjectionMatrix(float aspectRatio) const;

        // Set the distance for the orbiting camera
        void SetDistance(float distance) { cameraDistance = distance; }

        // Get current camera position
        glm::vec3 GetPosition() const;
        void ToggleMode();
        void HandleInput(float deltaTime, bool forward, bool backward, bool left, bool right,
            bool rotateLeft, bool rotateRight, bool zoomIn, bool zoomOut);
        void Move(float deltaTime, bool forward, bool backward, bool left, bool right);
        void SetSpeed(float speed);
        void UpdateDirection(float deltaX, float deltaY);
        CameraMode GetCameraMode() {
            return mode;
        }


    private:
        CameraMode mode;
        float cameraDistance;  // Distance from the scene center
        float cameraAngle;     // Angle around the y-axis in radians
        float cameraSpeed;     // Speed of rotation
        glm::vec3 position;  // Camera's position in world space
        glm::vec3 front;     // Direction the camera is facing
        glm::vec3 up;        // World up vector
        glm::vec3 right;     // Perpendicular to front and up
        float yaw;           // Horizontal rotation angle
        float pitch;         // Vertical rotation angle
    };
}