#include "Entity.h"

namespace Lengine {
	void Entity::applyMaterialsToAllSubmeshes(UUID materialID) {
		for (auto& matID : materialInstanceUUID) {
			matID.second = materialID;
		}
	}
	void Entity::applyMaterialToSubmesh(std::string submeshName, UUID id) {
		materialInstanceUUID[submeshName] = id;
	}
}
