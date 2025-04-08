#pragma once
#include <cstdint>
#include <string>

#include "UiElement.h"
#include "FontRendering/FontId.h"
#include "FontRendering/TextManager.h"
#include "Shaders/Shader.h"

namespace Engine::Ui
{
    class Text final : UiElement
    {
    private:
        uint32_t VertexBuffer = 0;
        uint32_t VertexArray = 0;
        int32_t VertexCount = 0;
        std::string String;
        Shaders::Shader Shader;
        FontId Font;

    public:
        Text();

    public:
        [[nodiscard]] const std::string& GetText() const
        {
            return String;
        }

        void SetText(const std::string& String)
        {
            this->String = String;
            UpdateMesh();
        }

        void SetFont(const FontId FontId)
        {
            this->Font = FontId;
            UpdateMesh();
        }

        void SetFont(const std::string& FontName)
        {
            Font = TextManager::GetFontId(FontName);
            UpdateMesh();
        }

        [[nodiscard]] Shaders::Shader GetShader() const
        {
            return Shader;
        }

        void SetShader(const Shaders::Shader& Shader)
        {
            this->Shader = Shader;
        }

        void Render() const override;

    private:
        void UpdateMesh();
    };
}
