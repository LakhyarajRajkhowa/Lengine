#pragma once

#include "../graphics/geometry/Model.h"
#include "../graphics/geometry/Gizmos.h"
#include "../graphics/renderer/Renderer.h"
#include "../resources/AssetManager.h"
#include "../resources/fileLoader.h"



namespace Lengine {
	class SceneRenderer {
	public:
		SceneRenderer(
			Camera3d& cam,
			SceneManager& scnMgr,
			AssetManager& assetmgr
		) :
			camera(cam),
			sceneManager(scnMgr),
			assetManager(assetmgr),
			gizmoRenderer(assetmgr,scnMgr, cam),
			renderer(cam, assetmgr )
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
		GizmoRenderer gizmoRenderer;

		
		Scene* activeScene;
		

	};
}