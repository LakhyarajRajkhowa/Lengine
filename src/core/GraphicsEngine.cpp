#include "GraphicsEngine.h"

namespace Lengine {

	GraphicsEngine::GraphicsEngine() :
		sceneRenderer(camera,  sceneManager, assetManager),
		inputHandler(camera, inputManager,  window, isRunning),
		imguiLayer(inputManager, isRunning),
		assetManager(settings),
		sceneManager(assetManager)
		
	{
		
	}
	void GraphicsEngine::initSettings() {
		settings.loadSettings();
	}
	void GraphicsEngine::run() {
		initSystems();
	}



	void GraphicsEngine::initSystems() {
		InitTimer();
		Lengine::init();
		window.create(
			settings.windowName,
			settings.windowWidth,
			settings.windowHeight,
			settings.windowMode
		);
		imguiLayer.init(
			window.getWindow(),
			window.getGlContext()
		);
		
		redirect = new OutputRedirect(logBuffer);
		editorLayer = new EditorLayer(
			logBuffer,
			sceneManager,
			camera,
			inputManager,
			assetManager,
			window	
		);
		camera.init(
			editorLayer->GetViewportPanel().GetViewportSize().x,
			editorLayer->GetViewportPanel().GetViewportSize().y,
			&inputManager,
			{ settings.cameraPosX, 5, settings.cameraPosZ },
			settings.cameraFov
		);
		sceneRenderer.init();
		sceneRenderer.preloadAssets();
		sceneRenderer.initScene();   
		mainLoop();

		imguiLayer.shutdown();
		window.quitWindow();
	}

	void GraphicsEngine::mainLoop() {
		isRunning = true;
		ViewportPanel& viewportPanel = editorLayer->GetViewportPanel();

		while (isRunning) {

			inputManager.update();
			assetManager.processGpuUploads();
			assetManager.syncAssetsToScene(*sceneManager.getActiveScene());
			inputHandler.handleInputs(imguiLayer, *editorLayer);
			imguiLayer.beginFrame();

			//  Framebuffer captures the frame of the game screen
			viewportPanel.GetFramebuffer().Bind();
			sceneRenderer.clearFrame({ 0.0f, 0.0f, 0.0f, 1.0f });
			sceneRenderer.renderScene(editorLayer->config);
			viewportPanel.GetFramebuffer().Unbind();

			editorLayer->OnImGuiRender();

			assetManager.drawLoadingScreens();

			imguiLayer.endFrame();

			window.swapBuffer();

		}
		
	}
	


}

