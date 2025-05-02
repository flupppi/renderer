module;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
export module Camera;
import std;
namespace Engine {


    export class Camera {
    public:
        enum class CameraMode {
            FPS,
            Orbit,
        };

        Camera(float distance = 10.0f, float angle = 0.0f, float speed = 2.0f, float aspectRatio = 16.0f / 9.0f): cameraDistance(distance), cameraAngle(angle), cameraSpeed(speed), pitch(0.0f), yaw(-90.0f),
            position(glm::vec3(0.0f, 0.0f, 3.0f)),
            front(glm::vec3(0.0f, 0.0f, -1.0f)),
            up(glm::vec3(0.0f, 1.0f, 0.0f)),
            mode(CameraMode::FPS) {
            // Default to FPS mode
            right = glm::normalize(glm::cross(front, up));

        }

        // Update camera angle and distance based on user input
        void Update(float deltaTime, bool rotateLeft, bool rotateRight, bool zoomIn, bool zoomOut) {

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

        // Get the view matrix based on the current camera position and orientation
        glm::mat4 GetViewMatrix() const {

            if (mode == CameraMode::FPS) {
                return glm::lookAt(position, position + front, up);
            }
            else if (mode == CameraMode::Orbit) {
                float camX = cameraDistance * sin(cameraAngle);
                float camZ = cameraDistance * cos(cameraAngle);
                glm::vec3 cameraPosition(camX, 3.0f, camZ);
                return glm::lookAt(cameraPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            }
            return glm::mat4(1.0f); // Identity matrix as fallbacks
        }

        // Get the projection matrix
        glm::mat4 GetProjectionMatrix(float aspectRatio) const {
            return glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
        }

        // Set the distance for the orbiting camera
        void SetDistance(float distance) { cameraDistance = distance; }

        // Get current camera position
        glm::vec3 GetPosition() const {
            float camX = cameraDistance * sin(cameraAngle);
            float camZ = cameraDistance * cos(cameraAngle);
            return glm::vec3(camX, 3.0f, camZ);
        }

        glm::vec3 getPosition() const {
            return position;
        }
        void ToggleMode() {
            if (mode == CameraMode::FPS)
                mode = CameraMode::Orbit;
            else
                mode = CameraMode::FPS;
        }
        void HandleInput(float deltaTime, bool forward, bool backward, bool left, bool right, bool up, bool down,
            bool rotateLeft, bool rotateRight, bool zoomIn, bool zoomOut);
        void Move(float deltaTime, bool forward, bool backward, bool left, bool right, bool up, bool down);
        void SetSpeed(float speed) {
            cameraSpeed = speed;

        }
        void UpdateDirection(float deltaX, float deltaY) {
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
        CameraMode GetCameraMode() {
            return mode;
        }
        std::string DebugMode() const {
            if (mode == CameraMode::FPS)
                return std::string("FPS Mode");
            else if (mode == CameraMode::Orbit)
                return std::string("Orbit Mode");
            else
              return std::string("Mode");
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


    void Camera::HandleInput(float deltaTime, bool forward, bool backward, bool left, bool right, bool up, bool down,
        bool rotateLeft, bool rotateRight, bool zoomIn, bool zoomOut) {
        if (mode == CameraMode::FPS) {
            Move(deltaTime, forward, backward, left, right, up, down);
        }
        else if (mode == CameraMode::Orbit) {
            Update(deltaTime, rotateLeft, rotateRight, zoomIn, zoomOut);
        }
    }

    void Camera::Move(float deltaTime, bool forward, bool backward, bool left, bool right, bool up, bool down) {
        float velocity = cameraSpeed * deltaTime;
        if (forward)
            position += front * velocity;
        if (backward)
            position -= front * velocity;
        if (left)
            position -= this->right * velocity;
        if (right)
            position += this->right * velocity;
		if (up)
			position += this->up * velocity;
		if (down)
			position -= this->up * velocity;
    }
}