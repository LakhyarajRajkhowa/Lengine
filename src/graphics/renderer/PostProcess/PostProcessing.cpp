#include "PostProcessing.h"

using namespace Lengine;



void PostProcessing::initHDR(uint32_t w, uint32_t h) {
    width = w;
    height = h;
    fullscreenQuad.init();

    initToneMappingResources();
    initBloom();
}


void PostProcessing::initToneMappingResources() {
	toneMapShader.compileShaders(
		Paths::Shaders + "hdrShader.vert",
		Paths::Shaders + "hdrShader.frag"
	);
	toneMapShader.linkShaders();
    toneMapShader.use();
    toneMapShader.setInt("hdrBuffer", 0);
    toneMapShader.setInt("bloomBlur", 1);
    
}

void PostProcessing::initBloom() {
    pingpong.Create(width, height);

    blurShader.compileShaders(
        Paths::Shaders + "blur.vert",
        Paths::Shaders + "blur.frag"
    );
    blurShader.linkShaders();

    blurShader.use();
    blurShader.setInt("image", 0);

}

void PostProcessing::renderToneMapping(const bool bloom, const float exposure) {

    toneMapShader.use();
    toneMapShader.setBool("bloom", bloom);
    toneMapShader.setFloat("exposure", exposure);

    fullscreenQuad.draw();

    toneMapShader.unuse();


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
