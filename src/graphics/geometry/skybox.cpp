#include "skybox.h"

using namespace Lengine;

void Skybox::init() {
    skyboxVertices = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    // ---------------- VAO / VBO ----------------
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        skyboxVertices.size() * sizeof(float),
        skyboxVertices.data(),
        GL_STATIC_DRAW
    );

    // position attribute (location = 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE,
        3 * sizeof(float),
        (void*)0
    );

    glBindVertexArray(0);

    // ---------------- Shader ----------------
    skyboxShader.compileShaders(
        Paths::Shaders + "cubemapShader.vert",
        Paths::Shaders + "cubemapShader.frag"
    );
    skyboxShader.linkShaders();

    skyboxShader.use();
    skyboxShader.setInt("skybox", SKYBOX_TEXTURE_INDEX);

}




void Skybox::render(const Camera3d& camera) {
    glDisable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);

    skyboxShader.use();

    glm::mat4 view = glm::mat4(glm::mat3(camera.getViewMatrix()));
    skyboxShader.setMat4("view", view);
    skyboxShader.setMat4("projection", camera.getProjectionMatrix());

    glBindVertexArray(VAO);

    glActiveTexture(SKYBOX_TEXTURE_UNIT);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);


    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    glActiveTexture(GL_TEXTURE0); // restore default
}
