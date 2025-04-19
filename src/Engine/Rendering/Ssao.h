#pragma once
#include <random>
#include <glm/vec3.hpp>

#include "ScreenQuad.h"
#include "Engine/EngineObjects/CameraRenderData.h"
#include "Shaders/Shader.h"

namespace Engine
{

    class Ssao final
    {
    private:
        static constexpr uint8_t KernelSize = 64;
        glm::vec3 Kernel[KernelSize];
        uint32_t FrameBuffer = 0;
        uint32_t ColorTexture = 0;
        uint32_t NoiseTexture = 0;
        Shaders::Shader Shader;
        int32_t ProjectionMatrixLocation;
        int32_t KernelLocation;
        Rendering::ScreenQuad ScreenQuad;

    public:
        Ssao();

    public:
        ~Ssao();

    public:
        [[nodiscard]] uint32_t GetColorTexture() const
        {
            return ColorTexture;
        }

    public:
        void Render(const CameraRenderData& CameraData, uint32_t NormalsTexture) const;

    private:
        void GenerateKernel(std::default_random_engine& Generator,
                            std::uniform_real_distribution<float>& Distribution);

        void LoadShader();

        void GenerateBuffers();

        void GenerateNoiseTexture(std::default_random_engine& Generator,
                                  std::uniform_real_distribution<float>& Distribution);
    };
}
