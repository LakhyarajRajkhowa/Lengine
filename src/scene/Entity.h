#pragma once

#include <string>

#include "../graphics/geometry/Mesh.h"
#include "../graphics/geometry/Model.h"
#include "../graphics/opengl/GLSLProgram.h"
#include "../graphics/material/Material.h"
#include "../graphics/camera/Camera3d.h"
#include "../scene/Transform.h"

#include "../utils/UUID.h"

namespace Lengine {
	class Entity {
	public:
		Entity(const std::string& n, UUID mID)
			: name(n), meshID(mID) {
		}
		
		const std::string& getName() const { return name; }
		void setName(const std::string& newName) { name = newName; }
		UUID getMeshID() const { return meshID; }
		void setMeshID(const UUID& id) { meshID = id; }

		void setTransform(const Transform& t) { transform = t;  }
		Transform& getTransform() {
			return transform;
		}
		const Transform& getTransform() const {
			return transform;
		};
		const glm::mat4& getTransformMatrix() const { return transform.getMatrix(); }
		
		bool isSelected = false;
		bool isMovable = true;
	private:
		std::string name;
		Transform transform;
		UUID meshID;


	};
}

