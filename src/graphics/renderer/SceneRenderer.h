#pragma once

#include "../graphics/geometry/Model.h"
#include "../graphics/geometry/Gizmos.h"
#include "../graphics/geometry/skybox.h"
#include "../graphics/renderer/ForwardRenderer.h"
#include "../graphics/shadowMaps/shadowMap.h"
#include "../graphics/shadowMaps/shadowCubeMap.h"
#include "../graphics/renderer/postProcess/PostProcessing.h"

#include "../resources/AssetManager.h"
#include "../resources/fileLoader.h"



namespace Lengine {
	class SceneRenderer {
	public:
		SceneRenderer(
			Camera3d& cam,
			ViewportPanel& vp,
			SceneManager& scnMgr,
			AssetManager& assetmgr,
			GizmoRenderer& gizmoRndr,
			EngineSettings& sett,
			RenderSettings& rndrSett,
			PostProcessing& pp
		) :
			camera(cam),
			sceneManager(scnMgr),
			assetManager(assetmgr),
			forwardRenderer(cam, assetmgr),
			gizmoRenderer(gizmoRndr),
			settings(sett),
			renderSettings(rndrSett),
			shadowMap(settings.shadowMapResolution),
			shadowCubeMap(settings.shadowMapResolution),
			viewport(vp),
			postProcess(pp)
		{
		}

		void init();
		void preloadAssets();
		void initScene();
		void renderShadowPass();

		void renderScene(const EditorConfig& editorConfig);
		void endFrame();

		ForwardRenderer forwardRenderer;


		void RenderFrame(const EditorConfig& config);
		void OnRenderSettingsChanged();

	private:
		Camera3d& camera;
		SceneManager& sceneManager;
		ViewportPanel& viewport;
		AssetManager& assetManager;
		GizmoRenderer& gizmoRenderer;
		EngineSettings& settings;
		RenderSettings& renderSettings;
		PostProcessing& postProcess;

		Scene* activeScene;

		Skybox skybox;
		ShadowMap shadowMap;
		ShadowCubeMap shadowCubeMap;
	private:
		// render paths
		void renderLDR(const EditorConfig& config);
		void renderMSAA_LDR(const EditorConfig& config);
		void renderHDR(const EditorConfig& config);
		void renderHDR_MSAA(const EditorConfig& config);

		// helpers
		void beginScenePass();
		void endScenePass();

	};
}