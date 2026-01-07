#include "PostProcessing.h"

using namespace Lengine;

PostProcessing::PostProcessing()
{
}

void PostProcessing::initHDR() {
    initToneMappingResources();
}
void FullscreenQuad::init() {
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
}

void FullscreenQuad::draw() {
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void PostProcessing::initToneMappingResources() {
	m_ToneMapShader.compileShaders(
		Paths::Shaders + "hdrShader.vert",
		Paths::Shaders + "hdrShader.frag"
	);
	m_ToneMapShader.linkShaders();

    m_FullscreenQuad.init();
    
}
void PostProcessing::drawExposureEditor()
{
    ImGui::Begin("Renderer");

    ImGui::Text("HDR Settings");
    ImGui::Separator();

    ImGui::SliderFloat(
        "Exposure",
        &m_Exposure,
        0.01f,
        5.0f,
        "%.1f"
    );

    ImGui::End();
}
void PostProcessing::renderToneMapping() {
   



    m_ToneMapShader.use();
    m_ToneMapShader.setFloat("exposure", m_Exposure);

    m_FullscreenQuad.draw();


}
