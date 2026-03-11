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
		Entity(UUID eID)
			: ID(eID)
		{
			
		}
		UUID getID() const { return ID; }
		void setID(const UUID& id) { ID = id; }



	private:	
		UUID ID;
	};
}

