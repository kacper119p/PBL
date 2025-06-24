#pragma once
#include "Material.h"
#include "Engine/Textures/Texture.h"
#include "Properties/MaterialProperty.h"
#include "Properties/TextureMaterialProperty.h"
#include "Shaders/Shader.h"

namespace Materials
{

    class VacuumIndicatorMaterial final : public Material
    {
    private:
        static Shaders::Shader DepthPass;
        static Shaders::Shader MainPass;
        static Shaders::Shader DirectionalShadowPass;
        static Shaders::Shader PointSpotShadowPass;

    private:
        Vector3MaterialProperty EmissiveColor;
        FloatMaterialProperty Fill;

    public:
        VacuumIndicatorMaterial(const glm::vec3& EmissiveColor, float Fill);

        VacuumIndicatorMaterial();

    public:
        void UseDepthPass() const override;

        void Use() const override;

        void UseDirectionalShadows() const override;

        void UsePointSpotShadows() const override;

    public:
        [[nodiscard]] glm::vec3 GetEmissiveColor() const
        {
            return EmissiveColor.GetValue();
        }

        void SetEmissiveColor(const glm::vec3& EmissiveColor)
        {
            VacuumIndicatorMaterial::EmissiveColor.SetValue(EmissiveColor);
        }

        [[nodiscard]] float GetFill() const
        {
            return Fill.GetValue();
        }

        void SetFill(const float Fill)
        {
            this->Fill.SetValue(Fill);
        }

#if EDITOR
        void DrawImGui() override;
#endif

        SERIALIZATION_EXPORT_MATERIAL(VacuumIndicatorMaterial)
    };

} // Materials
