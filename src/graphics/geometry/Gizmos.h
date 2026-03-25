#pragma once

#include "../assets/MeshRegistry.h"
#include "../resources/AssetManager.h"
#include "../scene/Scene.h"
#include "../scene/SceneManager.h"

#include "../editor/EditorSelection.h"

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
		Submesh* gizmoArrow = nullptr;
		GLSLProgram gizmoArrowShader;
		glm::vec3 axis;
		glm::vec4 color;
	};

	class GizmoRenderer {
	public:
		GizmoRenderer(
			AssetManager& asstmgr
		) :
			assetManager(asstmgr)
		{
			
		}
		void GizmoRenderer::drawSingleArrow(
			GizmoArrows& arrow,
			const glm::vec3& position,
			const float& size,
			const bool& highlight
		);

		void drawGizmoSpheres(Scene* activeScene, Camera3d* editorCamera);
		void drawGizmoGrid(
			const glm::mat4& view,
			const glm::mat4 proj,
			const glm::vec3 pos);
		void drawGizmoArrows(Scene* activeScene, Camera3d* editorCamera);
		void drawDebugCapsuleForArrow(
			Camera3d* editorCamera,
			const glm::vec3& position,
			const glm::vec3& axis,
			float size
		);

		AxisCapsule& getCapsule(const GizmoAxis axis);

		void InitGizmo() {
			initGizmoGrid();
			//initGizmoSpheres();
			//initGizmoArrows();
		}

	private:
		AssetManager& assetManager;

		Submesh* gizmoSphere = nullptr;
		GLSLProgram gizmoSphereShader;

		Submesh* gizmoGrid = nullptr;
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