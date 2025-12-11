#include "GraphicsEngine.h"

namespace Lengine {

	GraphicsEngine::GraphicsEngine() :
		sceneRenderer(camera,  sceneManager, assetManager),
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
			window,
			sceneRenderer.renderer	
		);
		camera.init(
			editorLayer->GetViewportPanel().GetViewportSize().x,
			editorLayer->GetViewportPanel().GetViewportSize().y,
			&inputManager,
			{ settings.cameraPosX, 5, settings.cameraPosZ },
			settings.cameraFov
		);

		sceneRenderer.init();
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
			inputHandler.handleInputs(imguiLayer, *editorLayer);
			imguiLayer.beginFrame();

			//  Framebuffer captures the frame of the game screen
			viewportPanel.GetFramebuffer().Bind();
			sceneRenderer.clearFrame({ 0.0f, 0.0f, 0.0f, 1.0f });
			sceneRenderer.renderScene();
			viewportPanel.GetFramebuffer().Unbind();

			editorLayer->OnImGuiRender();


			imguiLayer.endFrame();

			window.swapBuffer();

		}
		
	}
	


}

