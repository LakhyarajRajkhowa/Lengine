#include "Camera3d.h"


namespace Lengine {
    float _speed = 0.1f;
    Camera3d::Camera3d() {
        _applyGravity = false;
    }
    Camera3d::~Camera3d() {}

    void Camera3d::init(float width, float height, InputManager* inputManager, glm::vec3 cameraPos, float FOV) {
        position = cameraPos;
        yaw = -90.0f;
        pitch = 0.0f;
        nearPlane = 0.50f;
        farPlane = 1000.0f;
        up = glm::vec3(0.0f, 1.0f, 0.0f);

        fov = FOV;
        aspectRatio = width / height;
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
        return glm::lookAt(position, position + front, up);
    }
    const glm::mat4& Camera3d::getViewMatrix() const{
        return glm::lookAt(position, position + front, up);
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
   
    void Camera3d::update(const float& deltaTime, const glm::vec2& mouseCoords){

            speedMultiplier = _inputManager->isKeyDown(SDLK_LCTRL) ? 5.0f : 1.0f;
            const float speed =  deltaTime * speedFactor * speedMultiplier;
            moveMouse(mouseCoords.x, mouseCoords.y);
            moveKeyboard(speed);        
        }
        
    

    void Camera3d::moveMouse(float xoffset, float yoffset) {
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
        front = glm::normalize(direction);
      
    }

    void Camera3d::moveKeyboard(const float& speed) {
        for (SDL_Keycode key : { SDLK_w, SDLK_s, SDLK_a, SDLK_d, SDLK_SPACE, SDLK_LSHIFT,SDLK_ESCAPE}) {
            
            if (_inputManager->isKeyDown(key)) {
                switch (key) {
                
                case SDLK_w:
                    position += glm::normalize(front) * speed ;
                    break;
                case SDLK_s:
                    position -= glm::normalize(front) * speed;
                    break;
                case SDLK_a:
                    position -= glm::normalize(glm::cross(front, up)) * speed;
                    break;
                case SDLK_d:
                    position += glm::normalize(glm::cross(front, up)) * speed;
                    break;
                case SDLK_SPACE:
                    position += glm::normalize(up) * speed;
                    break;
                case SDLK_LSHIFT:
                    position -= glm::normalize(up) * speed;
                    break;
                case SDLK_ESCAPE:
                    isFixed = !isFixed;
                    break;
                case SDLK_c:
                    isFixed = !isFixed;
                    break;


                }
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
