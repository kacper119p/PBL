#include "ModelRenderer.h"
#include "Engine/EngineObjects/LightManager.h"
#include "Engine/EngineObjects/CameraRenderData.h"

namespace Engine
{
    ModelRenderer::ModelRenderer(Materials::Material* Material, Models::Model* Model) :
        Material(Material), Model(Model)
    {
    }

    void ModelRenderer::RenderDepth(const CameraRenderData& RenderData)
    {
        Material->UseDepthPass();

        SetupMatrices(RenderData, Material->GetDepthPass());
        Draw();
    }

    void ModelRenderer::Render(const CameraRenderData& RenderData)
    {
        Material->Use();

        LightManager::GetInstance()->SetupLightsForRendering(Material->GetMainPass());

        SetupMatrices(RenderData, Material->GetMainPass());
        Draw();
    }

    void ModelRenderer::RenderDirectionalShadows(const CameraRenderData& RenderData)
    {
        Material->UseDirectionalShadows();

        SetupMatrices(RenderData, Material->GetDirectionalShadowPass());
        Draw();
    }

    void
    ModelRenderer::RenderPointSpotShadows(const glm::vec3& LightPosition, float LightRange,
                                          const glm::mat4* const SpaceTransformMatrices)
    {
        Material->UsePointSpotShadows();


        Material->GetPointSpotShadowPass().SetUniform("ShadowMatrices[0]", SpaceTransformMatrices[0]);
        Material->GetPointSpotShadowPass().SetUniform("ShadowMatrices[1]", SpaceTransformMatrices[1]);
        Material->GetPointSpotShadowPass().SetUniform("ShadowMatrices[2]", SpaceTransformMatrices[2]);
        Material->GetPointSpotShadowPass().SetUniform("ShadowMatrices[3]", SpaceTransformMatrices[3]);
        Material->GetPointSpotShadowPass().SetUniform("ShadowMatrices[4]", SpaceTransformMatrices[4]);
        Material->GetPointSpotShadowPass().SetUniform("ShadowMatrices[5]", SpaceTransformMatrices[5]);

        Material->GetPointSpotShadowPass().SetUniform("LightPosition", LightPosition);
        Material->GetPointSpotShadowPass().SetUniform("Range", LightRange);

        Material->GetPointSpotShadowPass().SetUniform("ObjectToWorldMatrix",
                                                      GetOwner()->GetTransform()->GetLocalToWorldMatrix());

        Draw();
    }

    void ModelRenderer::SetupMatrices(const CameraRenderData& RenderData, const Shaders::Shader& Shader) const
    {
        Shader.SetUniform("CameraPosition", RenderData.CameraPosition);
        Shader.SetUniform("ViewMatrix", RenderData.ViewMatrix);
        Shader.SetUniform("ProjectionMatrix", RenderData.ProjectionMatrix);
        Shader.SetUniform("ObjectToWorldMatrix",
                          GetOwner()->GetTransform()->GetLocalToWorldMatrix());
    }

    void ModelRenderer::Draw() const
    {
        for (int i = 0; i < Model->GetMeshCount(); ++i)
        {
            Model->GetMesh(i)->Draw();
        }
    }
} // Engine
