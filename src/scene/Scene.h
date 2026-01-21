#pragma once

#include "../resources/TextureCache.h"

#include "../scene/Entity.h"
#include "../scene/components/MeshRendererStorage.h"
#include "../scene/components/MeshFilterStorage.h"
#include "../assets/MaterialRegistry.h"
namespace Lengine {
    class Scene {
    public:
        
        Scene(const std::string& n, UUID sID)
            : name(n), sceneID(sID) 
        {
        }
  
        Entity* createEntity(
            const std::string& name,
            const UUID meshID,
            EntityType type = EntityType::DefaultObject,
            UUID entityID = UUID()
        );

        Entity* addEntity(std::unique_ptr<Entity> entity, const UUID originalEntityId);
        
        const  Entity* getEntityByName(const std::string& name) const;
        Entity* getEntityByName(const std::string& name) ;

        const  Entity* getEntityByID(const UUID& id) const;
        Entity* getEntityByID(const UUID& UUID);

        const Entity* Scene::getEntityByIndex(const uint32_t& index) const {
            return entities[index].get();
        }
        Entity* Scene::getEntityByIndex(const uint32_t& index)  {
            return entities[index].get();
        }

        const std::vector<std::unique_ptr<Entity>>& getEntities() const { return entities; }
        std::vector<std::unique_ptr<Entity>>& Scene::getEntities() {
            return entities;
        }

        const std::vector<Light>& getLights() const { return lights; }
        std::vector<Light>& Scene::getLights() {
            return lights;
        }
        Light& getMainDirectionalLight() {
            for (auto& l : lights)
                if (l.type == LightType::Directional)
                    return l;
            Light directionalLight = Light();
            directionalLight.setType(LightType::Directional);
            directionalLight.direction = glm::vec3(0.5f);

            return directionalLight;
        }


        Light& getMainSpotLight() {
            for (auto& l : lights)
                if (l.type == LightType::Spotlight)
                    return l;
            Light light = Light();
            light.setType(LightType::Spotlight);

            return light;
        }
        Light& getMainPointLight() {
            for (auto& l : lights)
                if (l.type == LightType::Point)
                    return l;
            Light light = Light();
            light.setType(LightType::Point);

            return light;
        }
        const glm::vec3& getAmbientLighting() const { return ambient; }
        glm::vec3& getAmbientLighting() { return ambient; }
        void setAmbientLighting(const glm::vec3& amb) { ambient = amb; }
        void removeEntity(const UUID);
       
        /*
        // ---- MeshRenderer API ----
        void addMeshRenderer(uint32_t e, const MeshRendererComponent& mr) {
            meshRenderers[e] = mr;
        }

        bool hasMeshRenderer(uint32_t e) const {
            return meshRenderers.find(e) != meshRenderers.end();
        }

        MeshRendererComponent& getMeshRenderer(uint32_t e) {
            return meshRenderers.at(e);
        }

        const std::unordered_map<uint32_t, MeshRendererComponent>&
            getMeshRenderers() const {
            return meshRenderers;
        }
        */
        bool showBoundingSphere = false;
        const std::string& getName() const { return name; }
        void rename(const std::string newName) { name = newName; }
        std::string getName() { return name; }
        UUID getUUID() const { return sceneID; }


        MaterialInstance& getMaterialInstance(UUID id);
        PBRMaterialInstance& getPbrMaterialInstance(UUID id);

        const MaterialInstance& getMaterialInstance(UUID id) const;
        const PBRMaterialInstance& getPbrMaterialInstance(UUID id) const;

        UUID createMaterialInstance(UUID baseMaterial);
        UUID createPbrMaterialInstance(UUID baseMaterial);
        void destroyMaterialInstance(UUID id);
        void assignDefaultMaterials(Entity* entity,Mesh* mesh);
        void assignDefaultPBRMaterials(const UUID entityID, Mesh* mesh);
        const void assignDefaultMaterials(Entity* entity, Mesh* mesh) const ;
        ResolvedMaterial getMaterialForSubmesh(
            Entity* entity,
            const std::string& submeshName,
            Material* base);
        
        const MeshRendererStorage& MeshRenderers() const {
            return meshRenderers;
        }

        MeshRendererStorage& MeshRenderers() {
            return meshRenderers;
        }

        const MeshFilterStorage& MeshFilters() const {
            return meshFilters;
        }

        MeshFilterStorage& MeshFilters() {
            return meshFilters;
        }

    private:
        std::string name;
        UUID sceneID;
        std::vector<std::unique_ptr<Entity>> entities;
        std::vector<Light> lights;

        glm::vec3 ambient = {0.5f, 0.5f, 0.5f};
        std::unordered_map<UUID, MaterialInstance> materialInstances;
        std::unordered_map<UUID, PBRMaterialInstance> pbrMaterialInstances;

        MeshRendererStorage meshRenderers;
        MeshFilterStorage meshFilters;

    };
}

