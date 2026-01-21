#include "shadowMap.h"

using namespace Lengine;

void ShadowMap::init() {

    glGenFramebuffers(1, &shadowFBO);

    // Depth texture
    glGenTextures(1, &shadowDepthTex);
    glBindTexture(GL_TEXTURE_2D, shadowDepthTex);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOW_RES, SHADOW_RES, 0,
        GL_DEPTH_COMPONENT, GL_FLOAT, nullptr
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Prevent shadow edge artifacts
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // Attach to FBO
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D,
        shadowDepthTex,
        0
    );

    // No color buffer
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    depthShader.compileShaders(Paths::Shaders + "depthShader.vert", Paths::Shaders + "depthShader.frag");
    depthShader.linkShaders();
}


void ShadowMap::renderDepthMap(
    std::vector<std::unique_ptr<Entity>>& entities,
    MeshRendererStorage& mrs,
    Light& light,
    AssetManager& assetManager
) {

  

}
