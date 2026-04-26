#pragma once 

#include "../graphics/opengl/GLSLProgram.h"
#include "../graphics/opengl/GLTexture.h"
#include "../graphics/camera/Camera3d.h"
#include "../core/paths.h"
#include "../resources/ImageLoader.h"
namespace Lengine {
	class Skybox {
	public:
		void Init();
		void Render(const glm::mat4& view, const glm::mat4& projection);

		const GLTexture getCubemapTexture() { return cubemapTexture; }
		void setCubemapTexture(const GLTexture& tex) { cubemapTexture = tex; }
	private:
		GLSLProgram skyboxShader;
		unsigned int VAO, VBO;
		GLTexture cubemapTexture;
		std::vector<float> skyboxVertices;


	};


}