#pragma once
#include <cstdint>
#include <iostream>

#include <GL/glew.h>

class PingPongFramebuffer {
public:
    GLuint FBO[2];
    GLuint colorBuffer[2];

    void Create(uint32_t width, uint32_t height) {
        glGenFramebuffers(2, FBO);
        glGenTextures(2, colorBuffer);

        for (int i = 0; i < 2; i++) {
            glBindFramebuffer(GL_FRAMEBUFFER, FBO[i]);

            glBindTexture(GL_TEXTURE_2D, colorBuffer[i]);
            glTexImage2D(
                GL_TEXTURE_2D, 0, GL_RGBA16F,
                width, height, 0,
                GL_RGBA, GL_FLOAT, nullptr
            );

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glFramebufferTexture2D(
                GL_FRAMEBUFFER,
                GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_2D,
                colorBuffer[i],
                0
            );

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                std::cout << "Pingpong FBO incomplete\n";
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};

