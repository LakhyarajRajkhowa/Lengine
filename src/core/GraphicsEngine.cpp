#include "GraphicsEngine.h"

namespace Lengine {

	GraphicsEngine::GraphicsEngine() :
		sceneManager(assetManager),
		gizmoRenderer(assetManager, sceneManager, camera),
		sceneRenderer(camera,  sceneManager, assetManager, gizmoRenderer, settings),
		inputHandler(camera, inputManager,  window, isRunning),
		imguiLayer(inputManager, isRunning),
		assetManager(settings)
		
	{
		
	}
	void GraphicsEngine::initSettings() {
		settings.loadSettings();
	}
	void GraphicsEngine::run() {
		initSystems();
		mainLoop();
		shutDown();
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
			gizmoRenderer,
			camera,
			inputManager,
			assetManager,
			window,
			glm::vec2(
				settings.resolution_X,
				settings.resolution_Y
			)
		);
		camera.init(
			settings.resolution_X,
			settings.resolution_Y,
			&inputManager,
			{ settings.cameraPosX, 5, settings.cameraPosZ },
			settings.cameraFov
		);
		sceneRenderer.init();
		sceneRenderer.preloadAssets();
		sceneRenderer.initScene();   
		

		
	}

	void GraphicsEngine::mainLoop() {
		isRunning = true;
		ViewportPanel& viewportPanel = editorLayer->GetViewportPanel();

		viewportPanel.GetMSAAFramebuffer().setMSAASamples(settings.msaaSamples);
		viewportPanel.GetMSAAFramebuffer().Create();
		while (isRunning) {

			inputManager.update();
			assetManager.processGpuUploads();
			assetManager.syncAssetsToScene(*sceneManager.getActiveScene());
			inputHandler.handleInputs(imguiLayer, *editorLayer);
			sceneRenderer.renderShadowPass(
				glm::vec2(
					settings.resolution_X,
					settings.resolution_Y
				)
			);
			imguiLayer.beginFrame();
			
			
			//  Framebuffer captures the frame of the game screen
			viewportPanel.GetMSAAFramebuffer().Bind();
			
			sceneRenderer.clearFrame({ 0.0f, 0.0f, 0.0f, 1.0f });

			sceneRenderer.renderScene(editorLayer->config);

			viewportPanel.GetMSAAFramebuffer().Unbind();
			
			viewportPanel.GetMSAAFramebuffer().ResolveTo(viewportPanel.GetFramebuffer());

			editorLayer->OnImGuiRender();

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

