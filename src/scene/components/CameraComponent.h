#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class CameraComponent
{
public:

    enum class ProjectionType
    {
        Perspective = 0,
        Orthographic = 1
    };

    ProjectionType projectionType = ProjectionType::Perspective;

    float fov = 45.0f;
    float nearClip = 0.1f;
    float farClip = 1000.0f;
    float aspectRatio = 1.778f;

    float orthoSize = 10.0f;
    glm::mat4 projection = glm::mat4(1.0f);

public:
    CameraComponent() {
        recalculateProjection();
    }

    void recalculateProjection()
    {
        if (projectionType == ProjectionType::Perspective)
        {
            projection = glm::perspective(
                glm::radians(fov),
                aspectRatio,
                nearClip,
                farClip
            );
        }
        else
        {
            float left = -orthoSize * aspectRatio * 0.5f;
            float right = orthoSize * aspectRatio * 0.5f;
            float bottom = -orthoSize * 0.5f;
            float top = orthoSize * 0.5f;

            projection = glm::ortho(
                left, right,
                bottom, top,
                nearClip, farClip
            );
        }
    }

};