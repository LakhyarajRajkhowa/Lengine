#pragma once

#include <string>

#include "../graphics/geometry/Mesh.h"
#include "../graphics/geometry/Model.h"
#include "../graphics/opengl/GLSLProgram.h"
#include "../graphics/material/Material.h"
#include "../graphics/camera/Camera3d.h"
#include "../scene/components/Light.h"
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
		Entity(UUID eID, const std::string& n)
			: ID(eID), name(n)
		{
			
		}
		UUID getID() const { return ID; }
		void setID(const UUID& id) { ID = id; }
		const std::string& getName() const { return name; }
		void setName(const std::string& newName) { name = newName; }


		Entity* Entity::Clone() 
		{
			Entity* e = new Entity(UUID(), this->name + "_" + std::to_string(numCopies));

			numCopies++;
			return e;
		}


	private:
		
		UUID ID;
		std::string name;
		
		uint32_t numCopies = 1;

	};
}

