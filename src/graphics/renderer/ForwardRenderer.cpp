#include "ForwardRenderer.h"

#include "../logging/LogBuffer.h"
using namespace Lengine;




void ForwardRenderer::bindCameraUniforms(
    GLSLProgram& shader,
    const glm::mat4& model,
    Camera3d& camera
) {

    shader.setMat4("model", model); 
    shader.setMat4("view", camera.getViewMatrix());
    shader.setMat4("projection", camera.getProjectionMatrix());
    shader.setVec3("cameraPos", camera.getCameraPosition());
    shader.setVec3("viewPos", camera.getCameraPosition());

}



void ForwardRenderer::bindPBRLights(
    GLSLProgram& shader,
    const std::vector<Light>& lights
) {
    int count = (int)lights.size();


    for (int i = 0; i < count; i++) {

        shader.setVec3(
            "lightColors[" + std::to_string(i) + "]",
            lights[i].color * glm::vec3(1000)
        );
    }
}

void ForwardRenderer::bindPBRMaterial(
    GLSLProgram& shader,
    const ResolvedMaterial& mat
) {
    shader.setVec3("material.albedo", mat.albedo);
    shader.setFloat("material.metallic", mat.metallic);
    shader.setFloat("material.roughness", mat.roughness);
    shader.setFloat("material.ao", mat.ao);
    shader.setFloat("material.normalStrength", mat.normalStrength);
}




void ForwardRenderer::bindTexture(
    GLSLProgram& shader,
    AssetManager& assetManager,
    const UUID& texID,
    const bool UseTexture,
    const char* hasUniform,
    const char* samplerUniform,
    GLenum textureUnit
) {
    bool hasTexture = (texID != UUID::Null && UseTexture);
    shader.setBool(hasUniform, hasTexture);

    if (!UseTexture) return;

    glActiveTexture(textureUnit);



    if (!hasTexture) {
        glBindTexture(GL_TEXTURE_2D, 0);
        return;
    }

    GLTexture* tex = assetManager.getTexture(texID);
    if (!tex) {
        glBindTexture(GL_TEXTURE_2D, 0);
        return;
    }

    glBindTexture(GL_TEXTURE_2D, tex->id);

    shader.setInt(samplerUniform, textureUnit - GL_TEXTURE0);
}



void ForwardRenderer::drawSubMesh(
    Submesh& sm,
    GLSLProgram& shader
) {
    sm.draw();
}

void ForwardRenderer::drawSubMeshGroup(
    Mesh& mesh,
    const std::vector<uint32_t>& subMeshIDs,
    GLSLProgram& shader
) {
    for (uint32_t smID : subMeshIDs)
    {
        Submesh& sm = mesh.subMeshes[smID];
        drawSubMesh(sm, shader);
    }
}
void ForwardRenderer::drawMeshAllSubMeshes(
    Mesh& mesh,
    GLSLProgram& shader
)
{

    for (const Submesh& sm : mesh.subMeshes)
    {
            sm.draw();
    }

}

void ForwardRenderer::bindShadowMapUniforms(
    GLSLProgram& shader,
    ShadowMap& shadowMap,
    const TransformComponent& lightTransform,
    const glm::vec3& camPos
) {

    glm::mat4 lightSpaceProj =
        glm::ortho(
            -20.0f, 20.0f,
            -20.0f, 20.0f,
            shadowMap.nearPlane, shadowMap.farPlane
        );

    
    glm::vec3 lightDir = glm::normalize(lightTransform.localRotation * glm::vec3(0.0f, -1.0f, 0.0f));

    glm::vec3 center = camPos;  // anchor to camera

    glm::vec3 lightPos = center - lightDir * 20.0f; // move back along light dir

    glm::mat4 lightView = glm::lookAt(
        lightPos,
        center,
        glm::vec3(0, 1, 0)
    );



    glm::mat4 lightSpaceMat = lightSpaceProj * lightView;
    shader.setMat4(
        "lightSpaceMatrix",
        lightSpaceMat
    );

    glActiveTexture(GL_TEXTURE0 + static_cast<unsigned int>(TextureUnit::Shadow2D));
    glBindTexture(GL_TEXTURE_2D, shadowMap.getDepthTexture());
    shader.setInt("shadowMap", static_cast<unsigned int>(TextureUnit::Shadow2D));
}

void ForwardRenderer::bindPointShadowUniforms(
    GLSLProgram& shader,
    ShadowCubeMap& shadowCubeMap
) {
    glActiveTexture(GL_TEXTURE0 + static_cast<unsigned int>(TextureUnit::ShadowCube));
    glBindTexture(GL_TEXTURE_CUBE_MAP, shadowCubeMap.getDepthCubeMap());
    shader.setInt("shadowCubeMap", static_cast<unsigned int>(TextureUnit::ShadowCube));

    shader.setFloat("farPlane", shadowCubeMap.getFarPlane());
}

void ForwardRenderer::RenderScene_pbr(
    const RenderContext& ctx
) {
    const Scene* activeScene = ctx.scene;
    const Camera3d* camera = ctx.camera;

    GLSLProgram* pbrShader = assetManager.getShader(ShaderRegistry::universalPbr.name);
    pbrShader->use();

    pbrShader->setInt("irradianceMap", static_cast<unsigned int>(TextureUnit::Irradiance));
    pbrShader->setInt("prefilterMap", static_cast<unsigned int>(TextureUnit::Prefilter));
    pbrShader->setInt("brdfLUT", static_cast<unsigned int>(TextureUnit::BRDF_LUT));


    pbrShader->setInt("shadowMap", static_cast<unsigned int>(TextureUnit::Shadow2D));
    pbrShader->setInt("shadowCubeMap", static_cast<unsigned int>(TextureUnit::ShadowCube));

    const auto& entities = activeScene->getEntities();
    auto& meshRenderers = activeScene->MeshRenderers();
    auto& meshFilters = activeScene->MeshFilters();
    auto& lightComponents = activeScene->Lights();
    auto& transforms = activeScene->Transforms();
    auto& animations = activeScene->Animations();


    // Bind camera once (view & projection are global)
    pbrShader->setMat4("view", camera->getViewMatrix());
    pbrShader->setMat4("projection", camera->getProjectionMatrix());
    pbrShader->setVec3("cameraPos", camera->getCameraPosition());

    // bind pre-computed IBL data
    glActiveTexture(GL_TEXTURE0 + static_cast<unsigned int>(TextureUnit::Irradiance));
    glBindTexture(GL_TEXTURE_CUBE_MAP, ctx.irradianceMap.id);

    glActiveTexture(GL_TEXTURE0 + static_cast<unsigned int>(TextureUnit::Prefilter));
    glBindTexture(GL_TEXTURE_CUBE_MAP, ctx.prefilterMap.id);

    glActiveTexture(GL_TEXTURE0 + static_cast<unsigned int>(TextureUnit::BRDF_LUT));
    glBindTexture(GL_TEXTURE_2D, ctx.brdfLUTMap.id);


    if (lightComponents.GetDirectionalShadowCasteer() != UUID::Null
        && transforms.Has(lightComponents.GetDirectionalShadowCasteer())
        )
    {
        bindShadowMapUniforms(
            *pbrShader,
            *ctx.shadowMap,
            transforms.Get(lightComponents.GetDirectionalShadowCasteer()),
            camera->getCameraPosition()
        );
    }


    if (lightComponents.GetPointShadowCasteer() != UUID::Null
        && transforms.Has(lightComponents.GetPointShadowCasteer())
        )
    {

        bindPointShadowUniforms(
            *pbrShader,
            *ctx.shadowCubeMap
        );
    }

       
    uint32_t lightNum = 0;

    for (const auto& [id, light] : lightComponents.GetAll())
    {
        if (!transforms.Has(id))
            continue;

        if (lightNum >= MAX_LIGHTS)
            break;

        const Light& l = light;

        // ---------------- Common ----------------
        pbrShader->setInt(
            "lightTypes[" + std::to_string(lightNum) + "]",
            static_cast<int>(l.type)
        );

        pbrShader->setVec3(
            "lightColors[" + std::to_string(lightNum) + "]",
            l.color
        );

        pbrShader->setFloat(
            "lightIntensities[" + std::to_string(lightNum) + "]",
            l.intensity
        );

        pbrShader->setBool(
            "lightCastShadow[" + std::to_string(lightNum) + "]",
            light.castShadow
        );

        // ---------------- Point & Spot ----------------
        pbrShader->setFloat(
            "lightRanges[" + std::to_string(lightNum) + "]",
            l.range
        );

        // ---------------- Spot only ----------------
        pbrShader->setFloat(
            "lightInnerAngles[" + std::to_string(lightNum) + "]",
            glm::cos(glm::radians(l.innerAngle))
        );

        pbrShader->setFloat(
            "lightOuterAngles[" + std::to_string(lightNum) + "]",
            glm::cos(glm::radians(l.outerAngle))
        );


        const TransformComponent& t = transforms.Get(id);

        // ---------------- Position ----------------
        // Used by point & spot, ignored by directional
        pbrShader->setVec3(
            "lightPositions[" + std::to_string(lightNum) + "]",
            t.localPosition
        );

        // ---------------- Direction ----------------
        // Used by directional & spot
        glm::vec3 direction = glm::normalize(t.localRotation * glm::vec3(0.0f, -1.0f, 0.0f));

        pbrShader->setVec3(
            "lightDirections[" + std::to_string(lightNum) + "]",
            direction
        );

        ++lightNum;

       
    }

    pbrShader->setInt("lightCount", lightNum);


    for (auto& [entityID, mr] : meshRenderers.All()) {

        if (!transforms.Has(entityID)) continue;
        if (!meshFilters.Has(entityID)) continue;


        const TransformComponent& t = transforms.Get(entityID);
        const AnimationComponent* anim = animations.Get(entityID);
        const MeshFilter& mf = meshFilters.Get(entityID);

        if (mf.HasPendingSubmesh()) continue;
        if (mr.inst.baseMaterial.isNull()) continue;

        glm::mat4 model = t.worldMatrix;
        pbrShader->setMat4("model", model);

        Submesh* sm = nullptr;

        if (!mf.submeshID.isNull()) {
            sm = assetManager.GetSubmesh(mf.submeshID);
        }
       
        // Animation
        if (sm && anim && anim->currentAnimationID != UUID::Null && anim->finalBoneMatrices.size())
        {

            for (int i = 0; i < sm->bonePalette.size(); i++)
            {
                int globalID = sm->bonePalette[i];

                pbrShader->setMat4(
                    "finalBonesMatrices[" + std::to_string(i) + "]",
                    anim->finalBoneMatrices[globalID]
                );
            }


        }


        Material* mat = assetManager.GetMaterial(mr.inst.baseMaterial);

        if (!mat) continue;

        const MaterialInstance& inst = mr.inst;
        const ResolvedMaterial& finalMat = ResolveMaterial(*mat, inst);

        pbrShader->setVec3("material.albedo", finalMat.albedo);
        pbrShader->setFloat("material.metallic", finalMat.metallic);
        pbrShader->setFloat("material.roughness", finalMat.roughness);
        pbrShader->setFloat("material.ao", finalMat.ao);
        pbrShader->setFloat("material.normalStrength", finalMat.normalStrength);


        bindTexture(
            *pbrShader,
            assetManager,
            finalMat.map_albedo,
            inst.use_map_albedo,
            "material.hasAlbedoMap",
            "material.albedoMap",
            GL_TEXTURE0 + static_cast<unsigned int>(TextureUnit::Albedo)
        );

        bindTexture(
            *pbrShader,
            assetManager,
            finalMat.map_normal,
            inst.use_map_normal,
            "material.hasNormalMap",
            "material.normalMap",
            GL_TEXTURE0 + static_cast<unsigned int>(TextureUnit::Normal)
        );

        bindTexture(
            *pbrShader,
            assetManager,
            finalMat.map_ao,
            inst.use_map_ao,
            "material.hasAOMap",
            "material.aoMap",
            GL_TEXTURE0 + static_cast<unsigned int>(TextureUnit::AO)
        );

        bindTexture(
            *pbrShader,
            assetManager,
            finalMat.map_metallic,
            inst.use_map_metallic,
            "material.hasMetallicMap",
            "material.metallicMap",
            GL_TEXTURE0 + static_cast<unsigned int>(TextureUnit::Metallic)
        );

        bindTexture(
            *pbrShader,
            assetManager,
            finalMat.map_roughness,
            inst.use_map_roughness,
            "material.hasRoughnessMap",
            "material.roughnessMap",
            GL_TEXTURE0 + static_cast<unsigned int>(TextureUnit::Roughness)
        );

        bindTexture(
            *pbrShader,
            assetManager,
            finalMat.map_metallicRoughness,
            inst.use_map_metallicRoughness,
            "material.hasMetallicRoughnessMap",
            "material.metallicRoughnessMap",
            GL_TEXTURE0 + static_cast<unsigned int>(TextureUnit::MetallicRoughness)
        );


            if(sm) sm->draw();
              
                
    }

    pbrShader->unuse();
}


void ForwardRenderer::RenderScene_debug(
    const RenderContext& ctx
) {


    const Scene* activeScene = ctx.scene;
    const Camera3d* camera = ctx.camera;

    GLSLProgram* shader = assetManager.getShader(ShaderRegistry::debug.name);
    GLSLProgram* outlineShader = assetManager.getShader(ShaderRegistry::outline.name);

    const auto& entities = activeScene->getEntities();
    auto& meshRenderers = activeScene->MeshRenderers();
    auto& meshFilters = activeScene->MeshFilters();
    auto& lightComponents = activeScene->Lights();
    auto& transforms = activeScene->Transforms();

    if (debugViewMode == DebugView::Geometry)
    {
        // -------- SOLID PASS --------
        shader->use();

        shader->setInt("u_DebugMode", static_cast<int>(IRenderer::debugViewMode));
        shader->setMat4("view", camera->getViewMatrix());
        shader->setMat4("projection", camera->getProjectionMatrix());
        shader->setVec3("cameraPos", camera->getCameraPosition());

        for (auto& [entityID, mr] : meshRenderers.All())
        {
            if (!transforms.Has(entityID)) continue;
            if (!meshFilters.Has(entityID)) continue;

            const TransformComponent& t = transforms.Get(entityID);
            shader->setMat4("model", t.worldMatrix);

            Submesh* sm = assetManager.GetSubmesh(meshFilters.Get(entityID).submeshID);
            if (sm) sm->draw();
        }

        shader->unuse();


        // -------- WIREFRAME OVERLAY --------
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(1.5f);

        outlineShader->use();
        outlineShader->setMat4("view", camera->getViewMatrix());
        outlineShader->setMat4("projection", camera->getProjectionMatrix());

        for (auto& [entityID, mr] : meshRenderers.All())
        {
            if (!transforms.Has(entityID)) continue;
            if (!meshFilters.Has(entityID)) continue;

            const TransformComponent& t = transforms.Get(entityID);
            outlineShader->setMat4("model", t.worldMatrix);

            Submesh* sm = assetManager.GetSubmesh(meshFilters.Get(entityID).submeshID);
            if (sm) sm->draw();
        }

        outlineShader->unuse();

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        return;
    }



    shader->use();

    shader->setInt("u_DebugMode", static_cast<int>(IRenderer::debugViewMode));


    // Bind camera once (view & projection are global)
    shader->setMat4("view", camera->getViewMatrix());
    shader->setMat4("projection", camera->getProjectionMatrix());
    shader->setVec3("cameraPos", camera->getCameraPosition());


    for (auto& [entityID, mr] : meshRenderers.All()) {

        if (!transforms.Has(entityID)) continue;
        if (!meshFilters.Has(entityID)) continue;


        const TransformComponent& t = transforms.Get(entityID);
        const MeshFilter& mf = meshFilters.Get(entityID);

        if (mf.HasPendingSubmesh()) continue;
        if (mr.inst.baseMaterial.isNull()) continue;

        glm::mat4 model = t.worldMatrix;
        shader->setMat4("model", model);


        Material* mat = assetManager.GetMaterial(mr.inst.baseMaterial);

        if (!mat) continue;

        const MaterialInstance& inst = mr.inst;



        const ResolvedMaterial& finalMat = ResolveMaterial(*mat, inst);

        shader->setVec3("albedoColor", finalMat.albedo);
        shader->setFloat("normalStrength", finalMat.normalStrength);
        shader->setFloat("nearPlane", 0.1f);
        shader->setFloat("farPlane", 1000.0f);



        bindTexture(
            *shader,
            assetManager,
            finalMat.map_albedo,
            inst.use_map_albedo,
            "hasAlbedoMap",
            "albedoMap",
            GL_TEXTURE1
        );

        bindTexture(
            *shader,
            assetManager,
            finalMat.map_normal,
            inst.use_map_normal,
            "hasNormalMap",
            "normalMap",
            GL_TEXTURE2
        );


        // new
        if (!mf.submeshID.isNull()) {
            Submesh* sm = assetManager.GetSubmesh(mf.submeshID);
            if (sm) sm->draw();
        }

    }

    shader->unuse();

}


/*
 
 TODO : SSAO 

void ForwardRenderer::renderSceneDepthNormals(
    Scene& activeScene,
    const EditorConfig& editorConfig
) {
    auto depthNormalShader =
        assetManager.getShader("depthNormalShader.vert");

    depthNormalShader->use();

    // Camera matrices
    depthNormalShader->setMat4("uView", camera.getViewMatrix());
    depthNormalShader->setMat4("uProjection", camera.getProjectionMatrix());

    const auto& entities = activeScene.getEntities();

    for (const auto& entityPtr : entities) {

        Entity* entity = entityPtr.get();
        if (!entity || !entity->isVisible)
            continue;

        UUID meshID = entity->getMeshID();
        Mesh* mesh = assetManager.getMesh(meshID);
        if (!mesh || entity->hasPendingMesh())
            continue;

        glm::mat4 model = entity->getTransformMatrix();
        depthNormalShader->setMat4("uModel", model);

        // IMPORTANT: no materials, no textures
        for (auto& [matIndex, subMeshes] : mesh->materialGroups) {
            if (!subMeshes.size()) continue;

            drawSubMeshGroup(
                *mesh,
                subMeshes,
                *depthNormalShader,
                {} // no hovered submesh highlight
            );
        }
    }

    depthNormalShader->unuse();
}
*/

/*
void Renderer::initOutlineShader() {
    outlineShader.compileShaders(
        Paths::Shaders + "defaultShader.vert",
        Paths::Shaders + "outlineShader.frag"
    );

    outlineShader.linkShaders();
}

void Renderer::renderSelectionOutline(
    Entity& entity,
    Camera3d& camera,
    AssetManager& assetManager
)
{
    if (!entity.isSelected)
        return;

    Mesh* mesh = assetManager.getMesh(entity.getMeshID());
    if (!mesh || entity.hasPendingMesh())
        return;

    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);

    outlineShader.use();

    glm::mat4 outlineModel =
        glm::scale(entity.getTransformMatrix(), glm::vec3(1.03f));

    bindCameraUniforms(outlineShader, outlineModel, camera);
    outlineShader.setVec3("outlineColor", glm::vec3(1.0f, 0.85f, 0.25f));

    drawMeshAllSubMeshes(*mesh, outlineShader);

    outlineShader.unuse();

    glEnable(GL_DEPTH_TEST);
    glStencilMask(0xFF);
}
*/


/*
void Renderer::collectRenderData(Scene& scene, Camera3d& camera, AssetManager& assetManager) {

    for (auto& [entityIndex, mr] : scene.getMeshRenderers())
    {

        const Entity* entity = scene.getEntityByIndex(entityIndex);

        glm::mat4 model = entity->getTransformMatrix();

        Mesh* mesh = assetManager.getMesh(mr.meshID);
        if (!mesh) continue;

        for (size_t i = 0; i < mesh->subMeshes.size(); i++)
        {
            RenderKey key;
            key.shader = assetManager.getMaterial(mr.materials[i])->getShader();
            key.materialID = mr.materials[i];
            key.meshID = mr.meshID;
            key.subMeshIndex = i;

            batcher.submit(key, {
                entityIndex,
                model
                });
        }
    }

}

void Renderer::applyMaterialUniforms(GLSLProgram* shader,
    UUID materialID,
    AssetManager& assetManager)
{
    Material* mat = assetManager.getMaterial(materialID);
    if (!mat) return;

    shader->setVec3("material.Ka", mat->Ka);
    shader->setVec3("material.Kd", mat->Kd);
    shader->setVec3("material.Ks", mat->Ks);
    shader->setVec3("material.Ke", mat->Ke);
    shader->setFloat("material.Ns", mat->Ns);
}

void Renderer::flushBatches(Scene& scene,
    Camera3d& camera,
    AssetManager& assetManager)
{
    for (auto& [key, drawList] : batcher.batches)
    {
        GLSLProgram* shader = key.shader;
        if (!shader) continue;

        shader->use();

        // ---- Camera uniforms (once per batch) ----
        shader->setMat4("view", camera.getViewMatrix());
        shader->setMat4("projection", camera.getProjectionMatrix());
        shader->setVec3("cameraPos", camera.getCameraPosition());

        shader->setVec3("lightColor", lightColor);
        shader->setVec3("lightPos", lightPos);

        // ---- Material uniforms (once per batch) ----
        applyMaterialUniforms(shader, key.materialID, assetManager);

        // ---- Geometry ----
        Mesh* mesh = assetManager.getMesh(key.meshID);
        if (!mesh) continue;

        SubMesh& sm = mesh->subMeshes[key.subMeshIndex];

        // ---- Draw all entities in this batch ----
        for (const RenderCommand& cmd : drawList)
        {
            shader->setMat4("model", cmd.model);
            sm.draw();
        }

        shader->unuse();
    }

    batcher.clear(); // IMPORTANT: per-frame
}

void Renderer::renderSceneECS(Scene& scene, Camera3d& camera, AssetManager& assetManager) {

    collectRenderData(scene, camera, assetManager);
    flushBatches(scene, camera, assetManager);

}
*/