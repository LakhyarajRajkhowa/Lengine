#pragma once

#include <string>

#include "../graphics/geometry/Mesh.h"
#include "../graphics/geometry/Model.h"
#include "../graphics/opengl/GLSLProgram.h"
#include "../graphics/material/Material.h"
#include "../graphics/camera/Camera3d.h"
#include "../graphics/lightning/Light.h"
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
		Entity(UUID eID, const std::string& n, EntityType t)
			: ID(eID), name(n), type(t)
		{
			
		}
		UUID getID() const { return ID; }
		void setID(const UUID& id) { ID = id; }
		uint32_t getIndex()  const { return index; }
		void setIndex(const uint32_t& idx) { index = idx; }
		const std::string& getName() const { return name; }
		void setName(const std::string& newName) { name = newName; }
		const EntityType& getType() const { return type; }
		void setType(const EntityType& t);



		Entity* getParent() { return parent; }
		std::vector<Entity*> getChildrens() { return children; }


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

	
		bool hasLight() const { return light.has_value(); }
		Light& getLight() { return *light; }
		const Light& getLight() const { return *light; }
		void addLight() {
			light.emplace(); 
			light.value().id = ID;
		}

		Entity* Entity::Clone() 
		{
			Entity* e = new Entity(UUID(), this->name + "_" + std::to_string(numCopies), type);

			e->index = -1;
			e->isSelected = false;
			e->isDragged = false;

			e->transform.position = this->transform.position + glm::vec3(0.5f, 0.0f, 0.5f);
			e->transform.scale = this->transform.scale;

			numCopies++;
			return e;
		}


	private:
		
		UUID ID;
		uint32_t index;
		std::string name;
		EntityType type;
		Transform transform;
		std::optional<Light> light;
		


		Entity* parent = nullptr;
		std::vector<Entity*> children;


		bool pendingMesh = false;
		UUID pendingMeshID;	

		uint32_t numCopies = 1;

	};
}

