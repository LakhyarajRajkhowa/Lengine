#include "Entity.h"

namespace Lengine {
	void Entity::applyMaterialsToAllSubmeshes(UUID materialID) {
		for (auto& matID : materialIDs) {
			matID.second = materialID;
		}
	}
	void Entity::applyMaterialToSubmesh(std::string submeshName, UUID id) {
		materialIDs[submeshName] = id;
	}
}
