#pragma once
#include <memory>


#include "../resources/LoadOBJ.h"
#include "../resources/LoadingSystem.h"

#include "../utils/filePath.h"
#include "../utils/imGuiScreens.h"

namespace Lengine {
	class Model {
	public:
		bool loadModel(
			const std::string& name,
			const std::string& path,
			std::shared_ptr<Lengine::Mesh>& mesh
		);
		
		
	};
}