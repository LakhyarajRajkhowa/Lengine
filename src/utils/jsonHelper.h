#pragma once
#include "json.hpp"
#include "glm/glm.hpp"

namespace Lengine {
	using json = nlohmann::json;

	static json vec3ToJson(const glm::vec3& v)
	{
		return { v.x, v.y, v.z };
	}

}
