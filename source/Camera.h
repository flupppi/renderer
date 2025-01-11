#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
namespace Engine {

    class Camera {
    public:
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

    private:
        float cameraDistance;  // Distance from the scene center
        float cameraAngle;     // Angle around the y-axis in radians
        float cameraSpeed;     // Speed of rotation
        float pitch;           // Up/Down rotation if needed
    };
}