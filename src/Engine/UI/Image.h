#pragma once
#include "UiElement.h"
#include "Engine/Rendering/ScreenQuad.h"
#include "Engine/Textures/Texture.h"

namespace Materials
{
    class UiMaterial;
}

namespace Engine::Ui
{
    class Image final : public UiElement
    {
    private:
        Texture Texture;
        Materials::UiMaterial* Material = nullptr;
        Rendering::ScreenQuad Quad;

    public:
        Image();

    public:
        [[nodiscard]] Engine::Texture GetTexture() const
        {
            return Texture;
        }

        void SetTexture(const Engine::Texture& Texture)
        {
            this->Texture = Texture;
        }

        [[nodiscard]] Materials::UiMaterial* GetMaterial() const
        {
            return Material;
        }

        void SetMaterial(Materials::UiMaterial* const Material)
        {
            this->Material = Material;
        }

    public:
        void Render() override;

    };
}
