
/*

--------- NOT COMPLETED YET! ------------


*/

/*
#pragma once

#include "../graphics/renderer/GBuffer.h"
#include "../graphics/renderer/IRenderer.h"

namespace Lengine {
	class DeferredRenderer : public IRenderer{
	public:
		DeferredRenderer(Camera3d& cam,
			AssetManager& assetmgr);

		void render(
			Scene& scene,
			EditorConfig& editorConfig,
			ShadowMap& shadowMap,
			ShadowCubeMap& shadowCubeMap
		) override
		{
			renderScene(scene, editorConfig, shadowMap, shadowCubeMap);
		}

	private:
		GBuffer gBuffer;
		uint32_t width = 0, height = 0;

	private:
		Camera3d& camera;
		AssetManager& assetManager;

	private:
		void renderScene(
			Scene& activeScene,
			EditorConfig& editorConfig,
			ShadowMap& shadowMap,
			ShadowCubeMap& shadowCubeMap
		);
	};
}
*/