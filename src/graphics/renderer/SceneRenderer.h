#pragma once

#include "../graphics/geometry/Model.h"
#include "../graphics/geometry/Gizmos.h"
#include "../graphics/geometry/skybox.h"
#include "../graphics/geometry/HDREnvironment.h"
#include "../graphics/renderer/ForwardRenderer.h"
#include "../graphics/shadowMaps/shadowMap.h"
#include "../graphics/shadowMaps/shadowCubeMap.h"
#include "../graphics/renderer/postProcess/PostProcessing.h"

#include "../resources/AssetManager.h"
#include "../resources/fileLoader.h"

#include "../graphics/renderer/RenderPipeline.h"


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
			forwardRenderer(assetmgr),
			gizmoRenderer(gizmoRndr),
			settings(sett),
			renderSettings(rndrSett),
			shadowMap(settings.shadowMapResolution),
			shadowCubeMap(settings.shadowMapResolution),
			viewport(vp),
			postProcess(pp)
		{
		}

		void Init();
		void preloadAssets();
		void initScene();
		void renderShadowPass();

		void UpdateScene();
		void RenderScene(const RenderContext& ctx);
		void endFrame();

		ForwardRenderer forwardRenderer;

		void RenderFrame();
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

		HDREnvironment hdrSkybox;

		ShadowMap shadowMap;
		ShadowCubeMap shadowCubeMap;
	private:
		// render paths
		void renderLDR(const RenderContext& ctx);
		void renderMSAA_LDR(const RenderContext& ctx);
		void renderHDR(const RenderContext& ctx);
		void renderHDR_MSAA(const RenderContext& ctx);

		// helpers
		void beginScenePass();
		void endScenePass();

	};
}