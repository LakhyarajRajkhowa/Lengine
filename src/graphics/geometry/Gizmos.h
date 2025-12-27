#pragma once

#include "../assets/MeshRegistry.h"
#include "../resources/AssetManager.h"
#include "../scene/Scene.h"
#include "../scene/SceneManager.h"


namespace Lengine {

	enum class GizmoAxis {
		None,
		X,
		Y,
		Z
	};



	struct AxisCapsule {
		glm::vec3 start;
		glm::vec3 end;
		float radius;
		bool hovered = false;
	};

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
			const float& size,
			const bool& highlight
		);

		void drawGizmoSpheres();
		void drawGizmoGrid();
		void drawGizmoArrows();
		void drawDebugCapsuleForArrow(
			const glm::vec3& position,
			const glm::vec3& axis,
			float size
		);

		AxisCapsule& getCapsule(const GizmoAxis axis);

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

		AxisCapsule capX;
		AxisCapsule capY;
		AxisCapsule capZ;
		
		void initGizmoSpheres();
		void initGizmoGrid();
		void initGizmoArrows();
	};
}