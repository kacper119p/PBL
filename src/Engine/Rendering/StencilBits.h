#pragma once
#include <cstdint>

namespace Engine
{
    enum StencilBits : uint8_t
    {
        StencilBits_None = 0,
        StencilBits_ToolHighlight = 1 << 0,
        StencilBits_Layer1 = 1 << 1,
        StencilBits_Layer2 = 1 << 2,
        StencilBits_Layer3 = 1 << 3,
        StencilBits_Layer4 = 1 << 4,
        StencilBits_Layer5 = 1 << 5,
        StencilBits_Layer6 = 1 << 6,
        StencilBits_Layer7 = 1 << 7,
        StencilBits_All = 0xFF
    };
}
