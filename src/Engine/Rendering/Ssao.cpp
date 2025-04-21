#include "Ssao.h"

#include <chrono>
#include <random>

#include "Shaders/ShaderManager.h"
#include "Shaders/ShaderSourceFiles.h"
#include "glm/glm.hpp"

namespace Engine
{
    Ssao::Ssao()
    {
        LoadShader();
        GenerateBuffers();

        std::uniform_real_distribution<float> distribution(0.0, 1.0);
        std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());

        GenerateKernel(generator, distribution);
        GenerateNoiseTexture(generator, distribution);
    }

    Ssao::~Ssao()
    {
        glDeleteFramebuffers(1, &FrameBuffer);
        glDeleteTextures(1, &ColorTexture);
        glDeleteTextures(1, &NoiseTexture);
    }

    void Ssao::Render(const CameraRenderData& CameraData, const uint32_t NormalsTexture) const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
        Shader.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, NormalsTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, NoiseTexture);
        Shaders::Shader::SetUniform(ProjectionMatrixLocation, CameraData.ProjectionMatrix);
        Shaders::Shader::SetUniform(InverseProjectionMatrixLocation, glm::inverse(CameraData.ProjectionMatrix));
        ScreenQuad.Draw();
    }

    void Ssao::GenerateKernel(std::default_random_engine& Generator,
                              std::uniform_real_distribution<float>& Distribution)
    {
        glm::vec3 kernel[KernelSize];

        for (int i = 0; i < KernelSize; ++i)
        {
            glm::vec3 sample(
                    Distribution(Generator) * 2.0f - 1.0f,
                    Distribution(Generator) * 2.0f - 1.0f,
                    Distribution(Generator)
                    );
            sample = glm::normalize(sample);
            sample *= Distribution(Generator);

            float scale = static_cast<float>(i) / 64.0f;
            scale = glm::mix(0.1f, 1.0f, scale * scale);
            sample *= scale;

            kernel[i] = sample;
        }

        // Can be only set once as long as SSAO shader is not used anywhere else.
        Shader.Use();
        Shader.SetUniformArray("Kernel", kernel, KernelSize);
    }

    void Ssao::LoadShader()
    {
        Shader = Shaders::ShaderManager::GetShader(
                Shaders::ShaderSourceFiles("./res/shaders/SSAO/SSAO.vert", nullptr, "./res/shaders/SSAO/SSAO.frag"));
        ProjectionMatrixLocation = Shader.GetUniformLocation("ProjectionMatrix");
        InverseProjectionMatrixLocation = Shader.GetUniformLocation("InverseProjectionMatrix");
    }

    void Ssao::GenerateBuffers()
    {
        glGenFramebuffers(1, &FrameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);

        glGenTextures(1, &ColorTexture);
        glBindTexture(GL_TEXTURE_2D, ColorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 1920, 1080, 0, GL_RED, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ColorTexture, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Ssao::GenerateNoiseTexture(std::default_random_engine& Generator,
                                    std::uniform_real_distribution<float>& Distribution)
    {
        glm::vec3 ssaoNoise[16];

        for (unsigned int i = 0; i < 16; ++i)
        {
            glm::vec3 noise(
                    Distribution(Generator) * 2.0f - 1.0f,
                    Distribution(Generator) * 2.0f - 1.0f,
                    0.0f
                    );
            ssaoNoise[i] = glm::normalize(noise);
        }

        glGenTextures(1, &NoiseTexture);
        glBindTexture(GL_TEXTURE_2D, NoiseTexture);
        glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB16F, 4, 4, 0,GL_RGB,GL_FLOAT, &ssaoNoise[0]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
}
