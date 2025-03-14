#pragma once
#include "Engine/EngineObjects/CameraRenderData.h"
#include "Shaders/Shader.h"

namespace Materials
{
    class TransformMaterialPropertyBundle
    {
    private:
        int CameraPosition;
        int ViewMatrix;
        int ProjectionMatrix;
        int ObjectToWorldMatrix;

    public:
        explicit TransformMaterialPropertyBundle(const Shaders::Shader& Owner) :
            CameraPosition(glGetUniformLocation(Owner.GetId(), "CameraPosition")),
            ViewMatrix(glGetUniformLocation(Owner.GetId(), "ViewMatrix")),
            ProjectionMatrix(glGetUniformLocation(Owner.GetId(), "ProjectionMatrix")),
            ObjectToWorldMatrix(glGetUniformLocation(Owner.GetId(), "ObjectToWorldMatrix"))
        {
        }

    public:
        void Bind(const Engine::CameraRenderData& RenderData) const
        {
            Shaders::ShaderBase::SetUniform(CameraPosition, RenderData.CameraPosition);
            Shaders::ShaderBase::SetUniform(ViewMatrix, RenderData.ViewMatrix);
            Shaders::ShaderBase::SetUniform(ProjectionMatrix, RenderData.ProjectionMatrix);
            Shaders::ShaderBase::SetUniform(ObjectToWorldMatrix, ObjectToWorldMatrix);
        }
    };
} // Models
