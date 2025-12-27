#include "Model.h"

#include <memory>


namespace Lengine {
    
    bool Model::loadModel(
        const std::string& name,
        const std::string& path,
        std::shared_ptr<Lengine::Mesh>& mesh
        ) 
    {
        mesh = std::make_shared<Lengine::Mesh>();
        mesh->name = name;
                
       bool loaded = assimpLoader(path, *mesh);
        for (auto& sm : mesh->subMeshes)
            sm.setupMesh();

        mesh->computeBounds();
        return loaded;
    }

 
}