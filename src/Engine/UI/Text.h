#pragma once
#include <cstdint>
#include <string>

#include "UiElement.h"
#include "FontRendering/FontId.h"
#include "FontRendering/TextManager.h"
#include "Shaders/Shader.h"

namespace Engine::Ui
{
    class Text final : public UiElement
    {
    private:
        std::string String;
        Shaders::Shader Shader;

        uint32_t VertexBuffer = 0;
        uint32_t VertexArray = 0;
        int32_t VertexCount = 0;

        int32_t SizeUniformLocation = -1;
        int32_t TransformUniformLocation = -1;
        int32_t ColorUniformLocation = -1;

        glm::vec4 Color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

        FontId Font;
        bool IsDirty = false;

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
            IsDirty = true;
        }

        void SetFont(const FontId FontId)
        {
            this->Font = FontId;
            IsDirty = true;
        }

        void SetFont(const std::string& FontName)
        {
            Font = TextManager::GetFontId(FontName);
            IsDirty = true;
        }

        [[nodiscard]] Shaders::Shader GetShader() const
        {
            return Shader;
        }

        void SetShader(const Shaders::Shader& Shader)
        {
            this->Shader = Shader;
        }

        [[nodiscard]] glm::vec4 GetColor() const
        {
            return Color;
        }

        void SetColor(const glm::vec4& Color)
        {
            this->Color = Color;
        }

        void Render() override;

    private:
        void UpdateMesh();
    };
}
