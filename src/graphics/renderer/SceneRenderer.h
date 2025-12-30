#pragma once

#include "../graphics/geometry/Model.h"
#include "../graphics/geometry/Gizmos.h"
#include "../graphics/geometry/skybox.h"
#include "../graphics/renderer/Renderer.h"
#include "../graphics/renderer/shadowMap.h"
#include "../resources/AssetManager.h"
#include "../resources/fileLoader.h"



namespace Lengine {
	class SceneRenderer {
	public:
		SceneRenderer(
			Camera3d& cam,
			SceneManager& scnMgr,
			AssetManager& assetmgr,
			GizmoRenderer& gizmoRndr,
			EngineSettings& sett
		) :
			camera(cam),
			sceneManager(scnMgr),
			assetManager(assetmgr),
			renderer(cam, assetmgr ),
			gizmoRenderer(gizmoRndr),
			settings(sett),
			shadowMap(settings.shadowMapResolution)
		{
		}

		void init();
		void preloadAssets();
		void initScene();
		void clearFrame(const glm::vec4& clearColor);
		void renderShadowPass(glm::vec2 resolution);
		void renderScene(EditorConfig& editorConfig);
		void endFrame();
		Renderer renderer;

	private:
		Camera3d& camera;
		SceneManager& sceneManager;
		AssetManager& assetManager;
		GizmoRenderer& gizmoRenderer;
		EngineSettings& settings;
		
		Scene* activeScene;
		
		Skybox skybox;
		ShadowMap shadowMap;


	};
}