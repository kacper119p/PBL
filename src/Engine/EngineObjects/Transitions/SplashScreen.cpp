#include "SplashScreen.h"

#include "Engine/Rendering/SpriteQuad.h"
#include "Engine/Textures/Texture.h"
#include "Engine/Textures/TextureManager.h"
#include "GLFW/glfw3.h"
#include "Math/Easings.h"
#include "Shaders/Shader.h"
#include "Shaders/ShaderManager.h"

namespace Engine
{
    void SplashScreen::Play(GLFWwindow* const Window, const int32_t ScreenWidth, const int32_t ScreenHeight)
    {
        float totalTime = 0.0f;
        float lastFrame = static_cast<float>(glfwGetTime());
        const Texture texture = TextureManager::GetTexture("res/textures/Logos/TeamLogo.dds");
        const Shaders::Shader shader = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "res/shaders/SplashScreen/SplashScreen.vert", "", "res/shaders/SplashScreen/SplashScreen.frag"));

        shader.Use();

        const int32_t sizePropertyLocation = shader.GetUniformLocation("Size");
        const int32_t alphaPropertyLocation = shader.GetUniformLocation("Alpha");

        glViewport(0, 0, ScreenWidth, ScreenHeight);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glEnable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture.GetId());
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        while (totalTime < Duration && !glfwWindowShouldClose(Window))
        {
            if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS
                || glfwGetKey(Window, GLFW_KEY_SPACE) == GLFW_PRESS
                || glfwGetKey(Window, GLFW_KEY_ENTER) == GLFW_PRESS)
            {
                glClear(GL_COLOR_BUFFER_BIT);
                EndFrame(Window);
                return;
            }
            if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1))
            {
                GLFWgamepadstate state;
                if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state)
                    && (state.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_PRESS
                        || state.buttons[GLFW_GAMEPAD_BUTTON_B] == GLFW_PRESS))
                {
                    glClear(GL_COLOR_BUFFER_BIT);
                    EndFrame(Window);
                    return;
                }
            }

            const float currentFrame = static_cast<float>(glfwGetTime());
            const float deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;
            totalTime += deltaTime;

            const float size = Math::EaseOutSine(totalTime / Duration) * 0.125f + 0.375f;
            Shaders::Shader::SetUniform(sizePropertyLocation, size);

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

            Shaders::Shader::SetUniform(alphaPropertyLocation, alpha);

            glClear(GL_COLOR_BUFFER_BIT);
            SpriteQuad::Draw();

            EndFrame(Window);
        }

        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        TextureManager::DeleteTexture("res/Logos/TeamLogo.dds");
    }

    void SplashScreen::EndFrame(GLFWwindow* const Window)
    {
        glfwPollEvents();
        glfwMakeContextCurrent(Window);
        glfwSwapBuffers(Window);
    }
}
