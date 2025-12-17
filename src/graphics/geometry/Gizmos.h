#pragma once

#include "../assets/MeshRegistry.h"
#include "../resources/AssetManager.h"
#include "../scene/Scene.h"
#include "../scene/SceneManager.h"


namespace Lengine {
	struct GizmoArrows {
		Mesh* gizmoArrow = nullptr;
		GLSLProgram gizmoArrowShader;
		glm::vec3 axis;
		glm::vec4 color;
	};

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
		void GizmoRenderer::drawSingleArrow(
			GizmoArrows& arrow,
			const glm::vec3& position,
			float size
		);

		void drawGizmoSpheres();
		void drawGizmoGrid();
		void drawGizmoArrows();

		void initGizmo() {
			initGizmoGrid();
			initGizmoSpheres();
			initGizmoArrows();
		}

	private:
		AssetManager& assetManager;
		SceneManager& sceneManager;
		Camera3d& camera;

		Mesh* gizmoSphere = nullptr;
		GLSLProgram gizmoSphereShader;

		Mesh* gizmoGrid = nullptr;
		GLSLProgram gizmoGridShader;

		
		
		GizmoArrows arrowX;
		GizmoArrows arrowY;
		GizmoArrows arrowZ;
		
		void initGizmoSpheres();
		void initGizmoGrid();
		void initGizmoArrows();
	};
}