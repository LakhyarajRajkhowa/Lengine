#pragma once
#include <memory>


#include "../resources/LoadOBJ.h"

#include "../utils/filePath.h"
#include "../utils/imGuiScreens.h"

namespace Lengine {
	class Model {
	public:
		MeshProperties loadModel(
			const std::string& name,
			const std::string& path,
			std::shared_ptr<Lengine::Mesh>& mesh
		);
		void StartAsyncLoad(const std::string& path, std::shared_ptr<Lengine::Mesh> mesh);
	};
}