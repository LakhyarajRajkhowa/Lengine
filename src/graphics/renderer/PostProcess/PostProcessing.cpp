#include "PostProcessing.h"

using namespace Lengine;



void PostProcessing::InitHDRShaders() {
    fullscreenQuad.Init();

    hdrToneMappingShader.compileShaders(
        Paths::Shaders + "hdrShader.vert",
        Paths::Shaders + "hdrToneMapping.frag"
    );
    hdrToneMappingShader.linkShaders();

    hdrToneMappingShader.use();
    hdrToneMappingShader.setInt("hdrBuffer", 0);
    hdrToneMappingShader.setInt("bloomBlur", 1);
    hdrToneMappingShader.unuse();

    blurShader.compileShaders(
        Paths::Shaders + "blur.vert",
        Paths::Shaders + "blur.frag"
    );
    blurShader.linkShaders();

    blurShader.use();
    blurShader.setInt("image", 0);
    blurShader.unuse();


}


void PostProcessing::InitBloom(uint32_t width, uint32_t height) {
    pingpong.Create(width, height);

}

void PostProcessing::renderToneMapping(const bool bloom, const float exposure) {

    hdrToneMappingShader.use();
    hdrToneMappingShader.setFloat("exposure", exposure);
    hdrToneMappingShader.setBool("enableBloom", bloom);
    fullscreenQuad.draw();

    hdrToneMappingShader.unuse();


}



void PostProcessing::renderBloom(GLuint hdrColorBuffer, const float blurScale) {
    bool horizontal = true;
    bool firstIteration = true;

    blurShader.use();

    for (int i = 0; i < blurPasses; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpong.FBO[horizontal]);
        blurShader.setBool("horizontal", horizontal);
        blurShader.setFloat("blurScale", blurScale);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(
            GL_TEXTURE_2D,
            firstIteration
            ? hdrColorBuffer        // bright-pass
            : pingpong.colorBuffer[!horizontal]
        );

        fullscreenQuad.draw();

        horizontal = !horizontal;
        if (firstIteration) firstIteration = false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


}
