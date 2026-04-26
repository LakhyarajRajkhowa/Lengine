#pragma once
#include "../graphics/opengl/GLSLProgram.h"

class FullscreenQuad {
public:
    void Init();
    void draw();
private:
    GLuint quadVAO = 0;
    GLuint quadVBO = 0;
};