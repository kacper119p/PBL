#pragma once
#include "Engine/Rendering/ScreenQuad.h"
#include "GLFW/glfw3.h"

namespace Engine
{

    class SplashScreen
    {
    private:
        static constexpr float Duration = 3.0f;

        static constexpr float FadeInTime = Duration * 0.1f;
        static constexpr float FadeOutStart = Duration * 0.75f;
        static constexpr float FadeOutTime = Duration - FadeOutStart;

    public:
        static void Play(GLFWwindow* Window, int32_t ScreenWidth, int32_t ScreenHeight);

    private:
        static void EndFrame(GLFWwindow* Window);
    };

}
