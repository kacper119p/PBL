#pragma once

#include "ParticleEmitter.h"
#include "Shaders/ComputeShader.h"
#include "Models/Model.h"

namespace Engine
{
    class FireParticleEmitter final : public ParticleEmitter
    {
    private:
        struct alignas(16) Particle
        {
            glm::vec3 Position = glm::vec3(0.0f);
            float Scale = 0.0f;
            glm::vec3 Velocity = glm::vec3(0.0f);
            float Life = -1.0f;
        };

        static_assert(sizeof(Particle) == 32, "Size mismatch with GLSL buffer");

    private:
        static constexpr float MaxLifetime = 64.0f / 24.0f;
        static constexpr int32_t MaxParticleCount = 64;
        static constexpr float SpawnRate = MaxParticleCount / MaxLifetime;

        float Timer = 0.0f;

        uint32_t ParticlesBuffer = 0;
        uint32_t FreelistBuffer = 0;
        uint32_t SortedIndicesBuffer = 0;

        int32_t ParticlesToSpawnProperty = 0;
        int32_t DeltaTimeProperty = 0;
        int32_t RandomProperty = 0;

        int32_t ViewMatrixLocation;
        int32_t ProjectionMatrixLocation;
        int32_t ObjectToWorldMatrixLocation;

    public:
        FireParticleEmitter();

    public:
        ~FireParticleEmitter() override;

    public:
        void Start() override;

        void DispatchSpawnShaders(float DeltaTime) override;

        void DispatchUpdateShaders(float DeltaTime) override;

        void Render(const CameraRenderData& RenderData) override;

#if EDITOR
        void DrawImGui() override;
#endif
        SERIALIZATION_EXPORT_CLASS(FireParticleEmitter)
    };
}
