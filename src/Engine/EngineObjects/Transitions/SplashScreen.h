#pragma once
#include <string>
#include <vector>

#include "GLFW/glfw3.h"

namespace Engine
{

    class SplashScreen
    {
    private:
        struct IterativeLoader
        {
            std::vector<std::string> Paths;
            size_t Index = 0;
            uint8_t Stage = 0;

            bool Step();
        };

    private:
        static constexpr float Duration = 3.0f;

        static constexpr float FadeInTime = Duration * 0.25f;
        static constexpr float FadeOutStart = Duration * 0.75f;
        static constexpr float FadeOutTime = Duration - FadeOutStart;

    public:
        static void Play(GLFWwindow* Window, int32_t ScreenWidth, int32_t ScreenHeight);

    private:
        static void EndFrame(GLFWwindow* Window);
    };

}
