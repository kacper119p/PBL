#pragma once

#include "Material.h"
#include "Engine/Textures/Texture.h"
#include "Properties/TextureMaterialProperty.h"

namespace Materials
{

    class ReflectiveMaterial final : public Material
    {
    private:
        static Shaders::Shader DepthPass;
        static Shaders::Shader MainPass;
        static Shaders::Shader DirectionalShadowPass;
        static Shaders::Shader PointSpotShadowPass;

    private:
        TextureMaterialProperty EnvironmentMap;

    public:
        explicit ReflectiveMaterial(Engine::Texture EnvironmentMap);

        ReflectiveMaterial();

    public:
        [[nodiscard]] Engine::Texture GetEnvironmentMap() const
        {
            return EnvironmentMap.GetValue();
        }

        inline void SetEnvironmentMap(const Engine::Texture EnvironmentMap)
        {
            ReflectiveMaterial::EnvironmentMap.SetValue(EnvironmentMap);
        }

    public:
        void UseDepthPass() const override;

        void Use() const override;

        void UseDirectionalShadows() const override;

        void UsePointSpotShadows() const override;
#if EDITOR
        void DrawImGui() override;
#endif

        SERIALIZATION_EXPORT_MATERIAL(ReflectiveMaterial);
    };

} // Models
