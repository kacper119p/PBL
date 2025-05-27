#include "BloodManager.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

#include "Shaders/ShaderManager.h"
#include "Utility/AssertionsUtility.h"

namespace Engine
{
    BloodManager* BloodManager::Instance = nullptr;

    BloodManager::BloodManager(const Models::AABBox3& SceneBounds)
    {
        CHECK_MESSAGE(Instance == nullptr, "BloodManager already exists.");
        Instance = this;

        glGenFramebuffers(1, &FrameBuffer);
        glGenTextures(1, &ColorBuffer);

        glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);

        CHECK_MESSAGE(FrameBuffer != 0, "Failed to create framebuffer.");
        CHECK_MESSAGE(ColorBuffer != 0, "Failed to create color buffer.");

        glBindTexture(GL_TEXTURE_2D, ColorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, MaskSize, MaskSize,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        constexpr float borderColor[] = {0.0f, 0.0f, 0.0f, 0.0f};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ColorBuffer, 0);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        const glm::vec3 cameraPosition = glm::vec3(0.0f, SceneBounds.max.y, 0.0f);
        constexpr glm::vec3 origin = glm::vec3(0.0f, 0.0f, 0.0f);
        constexpr glm::vec3 upDirection = glm::vec3(0.0f, 1.0f, -1.0f);

        const glm::mat4 viewMatrix = glm::lookAt(cameraPosition, origin, upDirection);

        const glm::mat4 projectionMatrix = glm::ortho(SceneBounds.min.x, SceneBounds.max.x,
                                                      SceneBounds.min.z, SceneBounds.max.z,
                                                      0.0f, SceneBounds.max.y - SceneBounds.min.y);

        ViewProjectionMatrix = projectionMatrix * viewMatrix;

        MaskShader = Shaders::ShaderManager::GetShader(
                Shaders::ShaderSourceFiles("./res/shaders/Blood/BloodMask.vert",
                                           "",
                                           "./res/shaders/Blood/BloodMask.frag"));

        ViewProjectionMatrixUniformLocation = MaskShader.GetUniformLocation("ViewProjectionMatrix");
        ModelMatrixUniformLocation = MaskShader.GetUniformLocation("ModelMatrix");
        ColorUniformLocation = MaskShader.GetUniformLocation("Color");
    }

    BloodManager::~BloodManager()
    {
        if (Instance == this)
        {
            Instance = nullptr;
        }
        glDeleteFramebuffers(1, &FrameBuffer);
        glDeleteTextures(1, &ColorBuffer);
    }

    void BloodManager::Update(const float DeltaTime)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
        glViewport(0, 0, MaskSize, MaskSize);
        glEnable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glCullFace(GL_BACK);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);

        MaskShader.Use();
        Shaders::Shader::SetUniform(GetViewProjectionMatrixUniformLocation(), GetMaskSpaceTransform());
        for (const BloodStain* const bloodStain : BloodStains)
        {
            bloodStain->Draw();
        }
        BloodStains.clear();

        for (const BloodSource* const bloodSource : BloodSources)
        {
            bloodSource->Draw();
        }

        glBlendFuncSeparate(GL_ZERO, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
        if (Eraser != nullptr)
        {
            Eraser->Draw();
        }

        glDisable(GL_BLEND);
    }
}
