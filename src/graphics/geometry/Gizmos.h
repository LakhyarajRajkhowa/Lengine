#pragma once

#include "../resources/AssetManager.h"
#include "../scene/Scene.h"
#include "../scene/SceneManager.h"
namespace Lengine {
	class GizmoRenderer {
	public:
		GizmoRenderer(
			AssetManager& asstmgr,
			SceneManager& scnMgr,
			Camera3d& cam
		) :
			assetManager(asstmgr),
			sceneManager(scnMgr),
			camera(cam)
		{
			
		}
		void initGizmo() {
			initGizmoGrid();
			initGizmoSpheres();
		}
		void drawGizmoSpheres();
		void drawGizmoGrid();	

	private:
		AssetManager& assetManager;
		SceneManager& sceneManager;
		Camera3d& camera;

		Mesh* gizmoSphere = nullptr;
		GLSLProgram gizmoSphereShader;

		Mesh* gizmoGrid = nullptr;
		GLSLProgram gizmoGridShader;


		void initGizmoSpheres();
		void initGizmoGrid();
	};
}