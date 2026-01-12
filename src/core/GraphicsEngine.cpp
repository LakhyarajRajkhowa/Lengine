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
			window,
			renderSettings,
			glm::vec2(
				settings.resolution_X,
				settings.resolution_Y
			)
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
		scenesTobeLoaded.push_back("defaultScene");
		scenesTobeLoaded.push_back("sponza");


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

			inputManager.update();

			inputHandler.handleInputs(imguiLayer, editorLayer);

			assetManager.processGpuUploads();
			assetManager.syncAssetsToScene(*sceneManager.getActiveScene());

			sceneRenderer.renderShadowPass();
			imguiLayer.beginFrame();
			sceneRenderer.OnRenderSettingsChanged();

			sceneRenderer.RenderFrame(editorLayer.config);

			editorLayer.OnImGuiRender();

			assetManager.drawLoadingScreens();

			imguiLayer.endFrame();
			window.swapBuffer();

		}

	}

	void GraphicsEngine::shutDown() {
		imguiLayer.shutdown();
		window.quitWindow();
	}
}
