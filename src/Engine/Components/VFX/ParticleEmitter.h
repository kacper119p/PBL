#pragma once

#include "Engine/Components/Component.h"
#include "Engine/EngineObjects/CameraRenderData.h"
#include "Materials/Material.h"
#include "Shaders/ComputeShader.h"
#include "Shaders/Shader.h"

namespace Engine
{
    class ParticleEmitter : public Component
    {
    protected:
        Materials::Material* Material = nullptr;

    protected:
        Shaders::ComputeShader SpawnShader;
        Shaders::ComputeShader UpdateShader;

    public:
        ParticleEmitter() = default;

        ParticleEmitter(Materials::Material* Material, const Shaders::ComputeShader& SpawnShader,
                        const Shaders::ComputeShader& UpdateShader);

    public:
        ~ParticleEmitter() override;

    public:
        [[nodiscard]] const Shaders::ComputeShader& GetUpdateShader() const
        {
            return UpdateShader;
        }

        [[nodiscard]] const Shaders::ComputeShader& GetSpawnShader() const
        {
            return SpawnShader;
        }

        /**
         * @brief Sets material used by this emitter.
         * @param Material A new material.
         */
        void SetMaterial(Materials::Material* Material);

        /**
         * @brief Returns Material used by this emitter.
         */
        [[nodiscard]] Materials::Material* GetMaterial() const
        {
            return Material;
        }

        void Start() override;

        virtual void DispatchSpawnShaders(float DeltaTime) = 0;

        virtual void DispatchUpdateShaders(float DeltaTime) = 0;

        virtual void Render(const CameraRenderData& RenderData) = 0;

    protected:
        void SetupMatrices(const CameraRenderData& RenderData, const Shaders::Shader& Shader) const;
    };
}
