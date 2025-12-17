#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SDL2/SDL.h>

#include "../platform/InputManager.h"

namespace Lengine {
    class Camera3d {
    public:
        Camera3d();
        ~Camera3d();
        void init(float width, float height, InputManager* inputManager, glm::vec3 cameraPos, float FOV);
         const glm::vec3& getCameraPosition() { return position; }
         const glm::vec3& getCameraDirection() { return front; }
         void setAspectRatio(float aspect);
         float getAspectRatio() const { return aspectRatio; }
         glm::mat4 getViewMatrix();
         void setProjectionMatrix(glm::vec4 projection);
         glm::mat4 getProjectionMatrix();
         glm::vec3 getRightVector();
         glm::vec3 getForwardVector();
        void update(const float& deltaTime, const glm::vec2& mouseCoords);
        void moveMouse( float xoffset,  float yoffset);
        void moveKeyboard(const float& speed);

        bool isFixed = true;

        glm::vec3 Camera3d::getForward() const;
        glm::vec3 Camera3d::getRight() const;
        glm::vec3 Camera3d::getUp() const;
    private:
        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;
        glm::mat4 projectionMatrix;
        float yaw, pitch;
        float fov;
        float aspectRatio;
        float nearPlane, farPlane;

        glm::vec3 direction;
        InputManager* _inputManager;

        void applyGravity();
        bool _applyGravity;

        float speedFactor = 10.0f;
        float speedMultiplier = 1.0f;

    };

}