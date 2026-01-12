#pragma once
#include "../graphics/opengl/GLSLProgram.h"

class FullscreenQuad {
public:
    void init();
    void draw();
private:
    GLuint quadVAO = 0;
    GLuint quadVBO = 0;
};