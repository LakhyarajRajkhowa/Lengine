#pragma once

#include "../graphics/geometry/Model.h"
#include "../graphics/geometry/Gizmos.h"
#include "../graphics/geometry/skybox.h"
#include "../graphics/renderer/Renderer.h"
#include "../resources/AssetManager.h"
#include "../resources/fileLoader.h"



namespace Lengine {
	class SceneRenderer {
	public:
		SceneRenderer(
			Camera3d& cam,
			SceneManager& scnMgr,
			AssetManager& assetmgr,
			GizmoRenderer& gizmoRndr
		) :
			camera(cam),
			sceneManager(scnMgr),
			assetManager(assetmgr),
			renderer(cam, assetmgr ),
			gizmoRenderer(gizmoRndr)
		{
		}

		void init();
		void preloadAssets();
		void initScene();
		void clearFrame(const glm::vec4& clearColor);
		void renderScene(EditorConfig& editorConfig);
		void endFrame();
		Renderer renderer;

	private:
		Camera3d& camera;
		SceneManager& sceneManager;
		AssetManager& assetManager;
		GizmoRenderer& gizmoRenderer;

		
		Scene* activeScene;
		
		Skybox skybox;

	};
}