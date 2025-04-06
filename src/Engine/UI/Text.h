#pragma once
#include <cstdint>
#include <string>

#include "UiElement.h"
#include "FontRendering/FontId.h"
#include "Shaders/Shader.h"

namespace Engine::Ui
{
    class Text final : UiElement
    {
    private:
        uint32_t VertexBuffer;
        uint32_t VertexArray;
        int32_t VertexCount;
        Shaders::Shader Shader;
        FontId Font;

    public:
        Text(const std::string& Text, const std::string& FontName);

        void Render() const override;
    };
}
