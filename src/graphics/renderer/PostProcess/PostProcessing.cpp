#include "PostProcessing.h"

using namespace Lengine;



void PostProcessing::initHDR(uint32_t w, uint32_t h) {
    width = w;
    height = h;
    fullscreenQuad.init();

}


void PostProcessing::InitToneMappingResources() {
	hdrToneMappingShader.compileShaders(
		Paths::Shaders + "hdrShader.vert",
		Paths::Shaders + "hdrToneMapping.frag"
	);
	hdrToneMappingShader.linkShaders();
    hdrToneMappingShader.use();
    hdrToneMappingShader.setInt("hdrBuffer", 0);
    hdrToneMappingShader.unuse();
    
}

void PostProcessing::InitBloom() {
    pingpong.Create(width, height);

    blurShader.compileShaders(
        Paths::Shaders + "blur.vert",
        Paths::Shaders + "blur.frag"
    );
    blurShader.linkShaders();

    blurShader.use();
    blurShader.setInt("image", 0);
    blurShader.unuse();

    hdrBloomShader.compileShaders(
        Paths::Shaders + "hdrShader.vert",
        Paths::Shaders + "hdrBloom.frag"
    );
    hdrBloomShader.linkShaders();

    hdrBloomShader.use();
    hdrBloomShader.setInt("hdrBuffer", 0);
    hdrBloomShader.setInt("bloomBlur", 1);
    hdrBloomShader.unuse();

}

void PostProcessing::renderToneMapping(const bool bloom, const float exposure) {

    hdrToneMappingShader.use();
    hdrToneMappingShader.setFloat("exposure", exposure);

    fullscreenQuad.draw();

    hdrToneMappingShader.unuse();


}

void PostProcessing::renderBloomShader() {

    hdrBloomShader.use();

    fullscreenQuad.draw();

    hdrBloomShader.unuse();


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
