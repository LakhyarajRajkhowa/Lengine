#include "Camera3d.h"

namespace Lengine {
    float _speed = 0.1f;
    Camera3d::Camera3d() {
    }
    Camera3d::~Camera3d() {}

    void Camera3d::init(
        InputManager* inputManager,
        const uint32_t width,
        const uint32_t height,
        glm::vec3 cameraPos,
        float FOV
    ) {
        position = cameraPos;
        yaw = -90.0f;
        pitch = 0.0f;
        nearPlane = 0.50f;
        farPlane = 1000.0f;
        up = glm::vec3(0.0f, 1.0f, 0.0f);

        fov = FOV;
        aspectRatio = (static_cast<float>(width) / static_cast<float>(height));
        _inputManager = inputManager;

        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(direction);
        projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    }

    void Camera3d::setAspectRatio(float aspect)
    {
        aspectRatio = aspect;   
        projectionMatrix = glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
    }
    glm::mat4 Camera3d::getViewMatrix() {
        return viewMatrix;
    }
    const glm::mat4& Camera3d::getViewMatrix() const{
        return viewMatrix;
    }

    glm::mat4 Camera3d::getProjectionMatrix() {
        return projectionMatrix;
    }
    const glm::mat4& Camera3d::getProjectionMatrix() const {
        return projectionMatrix;
    }
    glm::vec3 Camera3d::getRightVector() {
        return glm::normalize(glm::cross(front, up));
    }

    glm::vec3 Camera3d::getForwardVector() {
        return glm::normalize(front);
    }
   
    void Camera3d::Update(const float& deltaTime, const glm::vec2& mouseCoords){
            
        if (isFixed) return;

            speedMultiplier = _inputManager->isKeyDown(SDLK_LCTRL) ? 5.0f : 1.0f;
            const float speed =  deltaTime * speedFactor * speedMultiplier;
            moveMouse(mouseCoords.x, mouseCoords.y, speed);
            controlMovement(speed);

            viewMatrix = glm::lookAt(position, position + front, up);
        }
        
    

    void Camera3d::moveMouse(float xoffset, float yoffset, float speed) {
      

        if (controlMode == CameraControlMode::first) {
            float sensitivity = 0.1f;
            xoffset *= sensitivity;
            yoffset *= sensitivity;

            yaw += xoffset;
            pitch -= yoffset;
            if (pitch > 89.0f) pitch = 89.0f;
            if (pitch < -89.0f) pitch = -89.0f;

            direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            direction.y = sin(glm::radians(pitch));
            direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        }
        if (controlMode == CameraControlMode::second) {
            if (_inputManager->isMouseButtonDown(SDL_BUTTON_RIGHT)) {

                float sensitivity = 0.1f;
                xoffset *= sensitivity;
                yoffset *= sensitivity;

                yaw += xoffset;
                pitch -= yoffset;
                if (pitch > 89.0f) pitch = 89.0f;
                if (pitch < -89.0f) pitch = -89.0f;

                direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                direction.y = sin(glm::radians(pitch));
                direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            }
            float scrollFactor = 10.0f;
            if (_inputManager->getScrollY()) {
                if(_inputManager->getScrollY() > 0.0f)
                    position += glm::normalize(front) * speed * scrollFactor;

                if (_inputManager->getScrollY() < 0.0f)
                    position -= glm::normalize(front) * speed * scrollFactor;

                _inputManager->resetScroll();

            }

        }

        front = glm::normalize(direction);
      
    }

    void Camera3d::controlMovement(const float& speed)
    {
        glm::vec3 right = glm::normalize(glm::cross(front, up));

        if (controlMode == CameraControlMode::first)
        {
            if (_inputManager->isKeyDown(SDLK_w))
                position += glm::normalize(front) * speed;

            if (_inputManager->isKeyDown(SDLK_s))
                position -= glm::normalize(front) * speed;

            if (_inputManager->isKeyDown(SDLK_a))
                position -= right * speed;

            if (_inputManager->isKeyDown(SDLK_d))
                position += right * speed;

            if (_inputManager->isKeyDown(SDLK_SPACE))
                position += glm::normalize(up) * speed;

            if (_inputManager->isKeyDown(SDLK_LSHIFT))
                position -= glm::normalize(up) * speed;
        }

        else if (controlMode == CameraControlMode::second)
        {
            if (_inputManager->isMouseButtonDown(SDL_BUTTON_RIGHT))
            {
                if (_inputManager->isKeyDown(SDLK_w))
                    position += glm::normalize(front) * speed;

                if (_inputManager->isKeyDown(SDLK_s))
                    position -= glm::normalize(front) * speed;

                if (_inputManager->isKeyDown(SDLK_a))
                    position -= right * speed;

                if (_inputManager->isKeyDown(SDLK_d))
                    position += right * speed;

                if (_inputManager->isKeyDown(SDLK_SPACE))
                    position += glm::normalize(up) * speed;

                if (_inputManager->isKeyDown(SDLK_LSHIFT))
                    position -= glm::normalize(up) * speed;
            }
        }

    }

    glm::vec3 Camera3d::getForward() const {
        return glm::normalize(front);   // or -view[2]
    }

    glm::vec3 Camera3d::getRight() const {
        return glm::normalize(glm::cross(getForward(), up));
    }

    glm::vec3 Camera3d::getUp() const {
        return glm::normalize(glm::cross(getRight(), getForward()));
    }
}
