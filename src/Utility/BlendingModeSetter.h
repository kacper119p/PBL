#pragma once
#include <glad/glad.h>

namespace Utility
{
    enum BlendingMode
    {
        BlendingModeAlpha,
        BlendingModeAdditive,
        BlendingModePremultiplied,
        BlendingModeMultiply,
        BlendingModeLighten,
    };

    inline void SetGlBlending(const BlendingMode Mode)
    {
        switch (Mode)
        {
            case BlendingModeAlpha:
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            case BlendingModeAdditive:
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            case BlendingModePremultiplied:
                glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            case BlendingModeMultiply:
                glBlendFunc(GL_DST_COLOR, GL_ZERO);
            case BlendingModeLighten:
                glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);

        }
    }
}
