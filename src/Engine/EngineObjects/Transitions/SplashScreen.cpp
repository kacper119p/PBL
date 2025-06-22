#include "SplashScreen.h"

#include "Engine/Rendering/SpriteQuad.h"
#include "Engine/Textures/Texture.h"
#include "Engine/Textures/TextureManager.h"
#include "GLFW/glfw3.h"
#include "Materials/Material.h"
#include "Math/Easings.h"
#include "Models/ModelManager.h"
#include "Shaders/Shader.h"
#include "Shaders/ShaderManager.h"
#include "Utility/FileUtilities.h"
#include "spdlog/spdlog.h"

namespace Engine
{
    bool SplashScreen::IterativeLoader::Step()
    {
        switch (Stage)
        {
            case 0:
            {
                Utility::FindFilesWithExtension("res/textures", ".dds", Paths);
                ++Stage;
                return false;
            }
            case 1:
            {
                Texture _ = TextureManager::GetTexture(Paths[Index].c_str());
                ++Index;
                if (Index == Paths.size())
                {
                    ++Stage;
                    Index = 0;
                }
                return false;
            }
            case 2:
            {
                Paths.clear();
                Utility::FindFilesWithExtension("res/models", ".fbx", Paths);
                ++Stage;
                return false;
            }
            case 3:
            {
                Models::Model* _ = Models::ModelManager::GetModel(Paths[Index].c_str());
                ++Index;
                if (Index == Paths.size())
                {
                    ++Stage;
                    Index = 0;
                    return false;
                }
                return false;
            }
            case 4:
            {
                Paths.clear();
                Utility::FindFilesWithExtension("res/materials", ".mat", Paths);
                ++Stage;
                return false;
            }
            case 5:
            {
                Materials::Material* _ = Materials::MaterialManager::GetMaterial(Paths[Index].c_str());
                ++Index;
                if (Index == Paths.size())
                {
                    ++Stage;
                    Index = 0;
                    return true;
                }
                return false;
            }
            default:
            {
                return true;
            }
        }
    }

    void SplashScreen::Play(GLFWwindow* const Window, const int32_t ScreenWidth, const int32_t ScreenHeight)
    {
        IterativeLoader loader;
        float totalTime = 0.0f;
        float lastFrame = static_cast<float>(glfwGetTime());
        const Texture texture = TextureManager::GetTexture("res/textures/Logos/TeamLogo.dds");
        const Shaders::Shader shader = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "res/shaders/SplashScreen/SplashScreen.vert", "", "res/shaders/SplashScreen/SplashScreen.frag"));

        shader.Use();

        const int32_t sizePropertyLocation = shader.GetUniformLocation("Size");
        const int32_t alphaPropertyLocation = shader.GetUniformLocation("Alpha");
        const int32_t imagePropertyLocation = shader.GetUniformLocation("Image");
        Shaders::Shader::SetTextureHandle(imagePropertyLocation, texture.GetHandleReadonly());

        while (totalTime < Duration && !glfwWindowShouldClose(Window))
        {
            if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS
                || glfwGetKey(Window, GLFW_KEY_SPACE) == GLFW_PRESS
                || glfwGetKey(Window, GLFW_KEY_ENTER) == GLFW_PRESS)
            {
                break;
            }
            if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1))
            {
                GLFWgamepadstate state;
                if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state)
                    && (state.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_PRESS
                        || state.buttons[GLFW_GAMEPAD_BUTTON_B] == GLFW_PRESS))
                {
                    break;
                }
            }

            float currentFrame;
            float deltaTime;
            do
            {
                currentFrame = static_cast<float>(glfwGetTime());
                deltaTime = currentFrame - lastFrame;
                loader.Step();
            } while (deltaTime < 1 / 40.0f);

            lastFrame = currentFrame;
            totalTime += deltaTime;

            const float size = Math::EaseOutSine(totalTime / Duration) * 0.125f + 0.375f;

            float alpha;
            if (totalTime <= FadeInTime)
            {
                alpha = Math::EaseOutSine(totalTime / FadeInTime);
            }
            else if (totalTime >= FadeOutStart)
            {
                alpha = 1.0f - Math::EaseOutCubic((totalTime - FadeOutStart) / FadeOutTime);
            }
            else
            {
                alpha = 1.0f;
            }

            shader.Use();
            glViewport(0, 0, ScreenWidth, ScreenHeight);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glEnable(GL_BLEND);
            glDisable(GL_DEPTH_TEST);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

            Shaders::Shader::SetUniform(sizePropertyLocation, size);
            Shaders::Shader::SetUniform(alphaPropertyLocation, alpha);

            glClear(GL_COLOR_BUFFER_BIT);
            SpriteQuad::Draw();

            glDisable(GL_BLEND);
            glEnable(GL_DEPTH_TEST);

            EndFrame(Window);
        }

        glClear(GL_COLOR_BUFFER_BIT);
        EndFrame(Window);

        spdlog::info("Splash screen animation finished.");
        while (!loader.Step())
        {
        }


        TextureManager::DeleteTexture("res/Logos/TeamLogo.dds");
    }

    void SplashScreen::EndFrame(GLFWwindow* const Window)
    {
        glfwPollEvents();
        glfwMakeContextCurrent(Window);
        glfwSwapBuffers(Window);
    }
}
