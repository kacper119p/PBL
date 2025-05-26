#pragma once

#include "Renderer.h"
#include "Engine/Components/Interfaces/IUpdateable.h"
#include "Shaders/ComputeShader.h"
#include "Models/Model.h"
#include "Shaders/Shader.h"

namespace Engine
{
    /**
     * @brief Renders single particle system.
     */
    class ParticleEmitter final : public Component
    {
    private:
        struct Particle
        {
            glm::vec4 Color = glm::vec4(0.0f);
            glm::vec4 Position = glm::vec4(0.0f);
            glm::vec4 Velocity = glm::vec4(0.0f);
            glm::vec4 Scale = glm::vec4(0.0f);
            glm::vec4 Acceleration = glm::vec4(0.0f);
            glm::vec4 Life = glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f);
        };

    public:
        struct EmitterSettings
        {
            float SpawnRate = 0.0f;
            Models::Model* Model = nullptr;
            glm::vec4 MinColor = glm::vec4(0.0f);
            glm::vec4 MaxColor = glm::vec4(0.0f);
            glm::vec3 MinOffset = glm::vec3(0.0f);
            glm::vec3 MaxOffset = glm::vec3(0.0f);
            glm::vec3 MinVelocity = glm::vec3(0.0f);
            glm::vec3 MaxVelocity = glm::vec3(0.0f);
            glm::vec3 MinScale = glm::vec3(0.0f);
            glm::vec3 MaxScale = glm::vec3(0.0f);
            glm::vec3 MinAccel = glm::vec3(0.0f);
            glm::vec3 MaxAccel = glm::vec3(0.0f);
            float MinLife = 0.0f;
            float MaxLife = 0.0f;

            EmitterSettings() = default;

            EmitterSettings(float SpawnRate, Models::Model* Model, const glm::vec4& MinColor, const glm::vec4& MaxColor,
                            const glm::vec3& MinOffset, const glm::vec3& MaxOffset, const glm::vec3& MinVelocity,
                            const glm::vec3& MaxVelocity, const glm::vec3& MinScale, const glm::vec3& MaxScale,
                            const glm::vec3& MinAccel, const glm::vec3& MaxAccel, float MinLife, float MaxLife);
        };

    private:
        Materials::Material* Material = nullptr;
        EmitterSettings Settings;
        int32_t MaxParticleCount = 0;

        float Timer = 0.0f;

        uint32_t ParticlesBuffer;
        uint32_t FreelistBuffer;

        uint32_t ParticlesToSpawnProperty;
        uint32_t DeltaTimeProperty;
        uint32_t RandomProperty;

        Shaders::ComputeShader SpawnShader;
        Shaders::ComputeShader UpdateShader;

    public:
        ParticleEmitter() = default;

        ParticleEmitter(Materials::Material* Material, const Shaders::ComputeShader& SpawnShader,
                        const Shaders::ComputeShader& UpdateShader, const EmitterSettings& EmitterSettings,
                        int MaxParticleCount);

    public:
        ~ParticleEmitter() override;

    public:
        [[nodiscard]] const EmitterSettings& GetSettings() const
        {
            return Settings;
        }

        void SetSettings(const EmitterSettings& Settings)
        {
            ParticleEmitter::Settings = Settings;
        }

        [[nodiscard]] const Shaders::ComputeShader& GetUpdateShader() const
        {
            return UpdateShader;
        }

        void SetUpdateShader(const Shaders::ComputeShader& UpdateShader)
        {
            ParticleEmitter::UpdateShader = UpdateShader;
        }

        [[nodiscard]] const Shaders::ComputeShader& GetSpawnShader() const
        {
            return SpawnShader;
        }

        void SetSpawnShader(const Shaders::ComputeShader& SpawnShader)
        {
            ParticleEmitter::SpawnShader = SpawnShader;
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

        void DispatchSpawnShaders(float DeltaTime);

        void DispatchUpdateShaders(float DeltaTime);

        void Render(const CameraRenderData& RenderData);

    private:
        void SetupMatrices(const CameraRenderData& RenderData, const Shaders::Shader& Shader) const;

        void SetEmitterSettingsUniforms(Shaders::ComputeShader Shader) const;
#if EDITOR
        void DrawImGui() override;
#endif
        SERIALIZATION_EXPORT_CLASS(ParticleEmitter)
    };
}
