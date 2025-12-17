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
	enum class EntityType {
		DefaultObject = 0,
		Light,
		Camera,
		COUNT
	};

	
	class Entity {
	public:
		Entity(UUID eID, const std::string& n, EntityType t, UUID mID)
			: ID(eID), name(n), type(t), meshID(mID)
		{
			
		}
		UUID getID() const { return ID; }
		void setID(const UUID& id) { ID = id; }
		uint32_t getIndex()  const { return index; }
		void setIndex(const uint32_t& idx) { index = idx; }
		const std::string& getName() const { return name; }
		void setName(const std::string& newName) { name = newName; }
		const EntityType& getType() const { return type; }
		void setType(const EntityType& t) { type = t; }
		UUID getMeshID() const { return meshID; }
		void setMeshID(const UUID& id) { meshID = id; }
		std::unordered_map<unsigned int, UUID>& getMaterialIndexUUIDs() { return materialIndexToUUID; }


		void setTransform(const Transform& t) { transform = t;  }
		Transform& getTransform() {
			return transform;
		}
		const Transform& getTransform() const {
			return transform;
		};
		const glm::mat4& getTransformMatrix() const { return transform.getMatrix(); }
		
		bool isSelected = false;
		bool isDragged = false;
		bool isMovable = true;
		bool isVisible = true;

		std::unordered_set<uint32_t> hoveredSubMeshes;
		std::unordered_set<uint32_t> selectedSubMeshes;

		
	private:
		
		UUID ID;
		uint32_t index;
		std::string name;
		EntityType type;
		Transform transform;

		UUID meshID;
		std::unordered_map<unsigned int, UUID> materialIndexToUUID;


		

	};
}

