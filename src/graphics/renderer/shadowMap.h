#pragma once
#include "../graphics/opengl/GLSLProgram.h"
#include "../scene/Scene.h"
#include "../resources/AssetManager.h"
#include "../core/paths.h"
namespace Lengine {
	class ShadowMap {
	public:
		ShadowMap(uint32_t shadowRes): SHADOW_RES(shadowRes) {}
		void init();
		void renderDepthMap(const Scene& scene,Light& light, AssetManager& assetManager );
		const GLuint& getDepthTexture() { return shadowDepthTex; }
	private:
		GLuint shadowFBO = 0;
		GLuint shadowDepthTex = 0;

		uint32_t SHADOW_RES = 1024;
		

		GLSLProgram depthShader;
	};
}