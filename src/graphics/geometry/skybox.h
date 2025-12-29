#pragma once 

#include "../graphics/opengl/GLSLProgram.h"
#include "../graphics/camera/Camera3d.h"
#include "../core/paths.h"
namespace Lengine {
	class Skybox {
	public:
		void init();
		void render(const Camera3d& camera);

		const GLint getCubemapTexture() { return cubemapTexture; }
		void setCubemapTexture(const unsigned int& tex) { cubemapTexture = tex; }
	private:
		GLSLProgram skyboxShader;
		GLuint VAO, VBO;
		unsigned int cubemapTexture;
		std::vector<float> skyboxVertices;

		const GLenum SKYBOX_TEXTURE_UNIT = GL_TEXTURE3;
		const GLint  SKYBOX_TEXTURE_INDEX = 3;

	};
}