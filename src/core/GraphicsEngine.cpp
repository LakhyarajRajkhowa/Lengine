#include "GraphicsEngine.h"

namespace Lengine {

	GraphicsEngine::GraphicsEngine() :
		window
		(
			settings.windowName,
			settings.windowWidth,
			settings.windowHeight,
			settings.windowMode
		),

		imguiLayer(
			inputManager,
			isRunning,
			window.getWindow(),
			window.getGlContext()
		),

		editorLayer(
			logBuffer,
			sceneManager,
			gizmoRenderer,
			camera,
			inputManager,
			assetManager,
			renderSettings,
			glm::vec2(
				settings.resolution_X,
				settings.resolution_Y
			),
			runtimeStats
		),

		sceneManager(assetManager),
		gizmoRenderer(assetManager, sceneManager, camera),

		sceneRenderer(
			camera,
			editorLayer.GetViewportPanel(),
			sceneManager,
			assetManager,
			gizmoRenderer,
			settings,
			renderSettings,
			postProcess
		),

		inputHandler(camera, inputManager, window, isRunning),
		
		assetManager(settings),
		postProcess(renderSettings)
	{

	}
	
	void GraphicsEngine::run() {
		initSystems();
		mainLoop();
		shutDown();
	}


	void GraphicsEngine::initSystems() {
		InitTimer();

		
		redirect = new OutputRedirect(logBuffer);
		
		camera.init(
			settings.resolution_X,
			settings.resolution_Y,
			&inputManager,
			{ settings.cameraPosX, 5, settings.cameraPosZ },
			settings.cameraFov
		);

		std::vector <std::string> scenesTobeLoaded;
		scenesTobeLoaded.push_back("emptyScene");

		sceneManager.loadScenes(scenesTobeLoaded);
		sceneRenderer.init();
		sceneRenderer.preloadAssets();
		sceneRenderer.initScene();

		postProcess.initHDR(settings.resolution_X, settings.resolution_Y);
	}

	void GraphicsEngine::mainLoop() {
		isRunning = true;
		ViewportPanel& viewportPanel = editorLayer.GetViewportPanel();


		while (isRunning) {
			
			UpdateTimer();

			inputManager.Update();
			assetManager.Update(*sceneManager.getActiveScene());
			inputHandler.handleInputs(imguiLayer, editorLayer, runtimeStats.frameStats.deltaTime);

			sceneRenderer.UpdateScene();

			sceneRenderer.renderShadowPass();
			imguiLayer.beginFrame();
			sceneRenderer.OnRenderSettingsChanged();

			sceneRenderer.RenderFrame(editorLayer.config);

			editorLayer.OnImGuiRender();

			assetManager.drawLoadingScreens();
			assetManager.drawImportingScreens();
			imguiLayer.endFrame();
			window.swapBuffer();

			

		}

	}


	void GraphicsEngine::UpdateTimer() {

		runtimeStats.frameStats	= LimitFPS(runtimeStats.targetFPS, runtimeStats.limitFPS);
		deltaTime = runtimeStats.frameStats.deltaTime;

	}

	void GraphicsEngine::shutDown() {

		// saving all imported assetmetadata to assetdb.json 
		// otherwise they will just be imported to librray and 
		// next time i load they wont show in asset panel
		assetManager.saveAssetDatabase();

		imguiLayer.shutdown();
		window.quitWindow();
	}
}
