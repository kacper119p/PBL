#include "Engine/EngineObjects/CameraRenderData.h"
#include "Engine/EngineObjects/LightManager.h"
#include "ModelRenderer.h"
#include "Models/ModelAnimated.h"
#include "Models/Animator.h"
#include "AnimatedModelRenderer.h"
#include <GLFW/glfw3.h>

namespace Engine
{
    AnimatedModelRenderer::AnimatedModelRenderer(Materials::Material* Material, Models::ModelAnimated* Model,
                                 Models::Animation* Animation, Models::Animator* Animator) :
        Material(Material), Model(Model), Animation(Animation), Animator(Animator)
    {
    }
    void AnimatedModelRenderer::RenderDepth(const CameraRenderData& RenderData)
    {
        Material->UseDepthPass();

        SetupMatrices(RenderData, Material->GetDepthPass());
        Draw();
    }

    void AnimatedModelRenderer::Render(const CameraRenderData& RenderData)
    {
        Material->Use();

        LightManager::GetInstance()->SetupLightsForRendering(Material->GetMainPass());

        SetupMatrices(RenderData, Material->GetMainPass());
        Draw();
    }

    void AnimatedModelRenderer::RenderDirectionalShadows(const CameraRenderData& RenderData)
    {
        Material->UseDirectionalShadows();

        SetupMatrices(RenderData, Material->GetDirectionalShadowPass());
        Draw();
    }

    void AnimatedModelRenderer::RenderPointSpotShadows(const glm::vec3& LightPosition, float LightRange,
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

    void AnimatedModelRenderer::SetupMatrices(const CameraRenderData& RenderData, const Shaders::Shader& Shader) const
    {
        Shader.SetUniform("CameraPosition", RenderData.CameraPosition);
        Shader.SetUniform("ViewMatrix", RenderData.ViewMatrix);
        Shader.SetUniform("ProjectionMatrix", RenderData.ProjectionMatrix);
        Shader.SetUniform("ObjectToWorldMatrix", GetOwner()->GetTransform()->GetLocalToWorldMatrix());
        std::vector<glm::mat4> transforms = Animator->GetFinalBoneMatrices();
        for (int i = 0; i < transforms.size(); ++i)
        {
            glm::mat4 transform = transforms[i];
            Shader.SetUniform(("finalBonesMatrices[" + std::to_string(i) + "]").c_str(), transform);
        }
    }

    void AnimatedModelRenderer::Draw() const
    {
        for (int i = 0; i < Model->GetMeshCount(); ++i)
        {
            Model->GetMesh(i)->Draw();
        }
    }
    

    void AnimatedModelRenderer::Update(float DeltaTime) { Animator->UpdateAnimation(DeltaTime); }
} 
