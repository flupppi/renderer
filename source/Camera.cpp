module;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
module Camera;

namespace Engine {
    Camera::Camera(float distance, float angle, float speed, float aspectRatio): cameraDistance(distance), cameraAngle(angle), cameraSpeed(speed), pitch(0.0f), yaw(-90.0f),
        position(glm::vec3(0.0f, 0.0f, 3.0f)),
        front(glm::vec3(0.0f, 0.0f, -1.0f)),
        up(glm::vec3(0.0f, 1.0f, 0.0f)),
        mode(CameraMode::FPS) {  // Default to FPS mode
        right = glm::normalize(glm::cross(front, up));
    }
    // Update camera based on user input
    void Camera::Update(float deltaTime, bool rotateLeft, bool rotateRight, bool zoomIn, bool zoomOut)
    {
        if (mode == CameraMode::FPS) {
            // FPS-style movement is handled separately
        }
        else if (mode == CameraMode::Orbit) {
            if (rotateLeft)
                cameraAngle -= cameraSpeed * deltaTime;
            if (rotateRight)
                cameraAngle += cameraSpeed * deltaTime;

            if (zoomIn)
                cameraDistance -= cameraSpeed * deltaTime;
            if (zoomOut)
                cameraDistance += cameraSpeed * deltaTime;

            // Keep camera angle within 0 to 2π range
            if (cameraAngle > glm::two_pi<float>())
                cameraAngle -= glm::two_pi<float>();
            else if (cameraAngle < 0.0f)
                cameraAngle += glm::two_pi<float>();

            // Ensure a minimum distance
            if (cameraDistance < 1.0f)
                cameraDistance = 1.0f;
        }
    }

    // Get the view matrix
    glm::mat4 Camera::GetViewMatrix() const
    {
        if (mode == CameraMode::FPS) {
            return glm::lookAt(position, position + front, up);
        }
        else if (mode == CameraMode::Orbit) {
            float camX = cameraDistance * sin(cameraAngle);
            float camZ = cameraDistance * cos(cameraAngle);
            glm::vec3 cameraPosition(camX, 3.0f, camZ);
            return glm::lookAt(cameraPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        return glm::mat4(1.0f); // Identity matrix as fallback
    }

    void Camera::HandleInput(float deltaTime, bool forward, bool backward, bool left, bool right,
        bool rotateLeft, bool rotateRight, bool zoomIn, bool zoomOut) {
        if (mode == CameraMode::FPS) {
            Move(deltaTime, forward, backward, left, right);
        }
        else if (mode == CameraMode::Orbit) {
            Update(deltaTime, rotateLeft, rotateRight, zoomIn, zoomOut);
        }
    }

    void Camera::Move(float deltaTime, bool forward, bool backward, bool left, bool right) {
        float velocity = cameraSpeed * deltaTime;
        if (forward)
            position += front * velocity;
        if (backward)
            position -= front * velocity;
        if (left)
            position -= this->right * velocity;
        if (right)
            position += this->right * velocity;
    }

    // Get the projection matrix
    glm::mat4 Camera::GetProjectionMatrix(float aspectRatio) const
    {
        return glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
    }

    // Get the current camera position (useful for lighting calculations, etc.)
    glm::vec3 Camera::GetPosition() const
    {
        float camX = cameraDistance * sin(cameraAngle);
        float camZ = cameraDistance * cos(cameraAngle);
        return glm::vec3(camX, 3.0f, camZ);
    }
    void Camera::ToggleMode() {
        if (mode == CameraMode::FPS)
            mode = CameraMode::Orbit;
        else
            mode = CameraMode::FPS;
    }
    void Camera::SetSpeed(float speed) {
        cameraSpeed = speed;
    }
    void Camera::UpdateDirection(float deltaX, float deltaY) {
        float sensitivity{ 0.1f };  // Adjust for finer or coarser control
        deltaX *= sensitivity;
        deltaY *= sensitivity;

        yaw += deltaX;
        pitch += deltaY;

        // Clamp the pitch to prevent flipping
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        // Calculate the new front vector
        glm::vec3 newFront{ 0,0,0 };
        newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        newFront.y = sin(glm::radians(pitch));
        newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(newFront);

        // Recalculate right and up vectors
        this->right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
        up = glm::normalize(glm::cross(this->right, front));
    }
}