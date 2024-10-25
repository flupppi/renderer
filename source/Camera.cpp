#include "Camera.h"

Camera::Camera(float distance, float angle, float speed, float aspectRatio)
    : cameraDistance(distance), cameraAngle(angle), cameraSpeed(speed), pitch(0.0f) {}

// Update camera based on user input
void Camera::Update(float deltaTime, bool rotateLeft, bool rotateRight, bool zoomIn, bool zoomOut)
{
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

// Get the view matrix
glm::mat4 Camera::GetViewMatrix() const
{
    float camX = cameraDistance * sin(cameraAngle);
    float camZ = cameraDistance * cos(cameraAngle);

    glm::vec3 cameraPosition(camX, 3.0f, camZ);
    return glm::lookAt(cameraPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
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