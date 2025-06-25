#pragma once
#include <cstdint>

#include "Shaders/Shader.h"

namespace Engine
{

    class TransitionTint
    {
    private:
        static Shaders::Shader BlendShader;
        static int32_t TintUniformLocation;
        static glm::vec4 TintColor;
        static bool Enabled;

    private:
        TransitionTint() = default;

    public:
        static void Initialize();

        static void Render();

        static void Enable()
        {
            Enabled = true;
        }

        static void Disable()
        {
            Enabled = false;
        }

        [[nodiscard]] static bool IsEnabled()
        {
            return Enabled;
        }

        static void SetTintColor(const glm::vec4& Color)
        {
            TintColor = Color;
        }

        static void GetTintColor(glm::vec4& Color)
        {
            Color = TintColor;
        }
    };

} // Engine
