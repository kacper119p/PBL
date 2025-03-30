#pragma once

#include "Material.h"
#include "Engine/Textures/Texture.h"

namespace Materials
{

    class ReflectiveMaterial final : public Material
    {
    public:
        static const std::string TypeName;

    private:
        static Shaders::Shader DepthPass;
        static Shaders::Shader MainPass;
        static Shaders::Shader DirectionalShadowPass;
        static Shaders::Shader PointSpotShadowPass;

    private:
        Engine::Texture EnvironmentMap;

    public:
        explicit ReflectiveMaterial(Engine::Texture EnvironmentMap);

        ReflectiveMaterial();

    public:
        static void Initialize();

    public:
        [[nodiscard]] Engine::Texture GetEnvironmentMap() const
        {
            return EnvironmentMap;
        }

        inline void SetEnvironmentMap(Engine::Texture EnvironmentMap)
        {
            ReflectiveMaterial::EnvironmentMap = EnvironmentMap;
        }

    public:
        void UseDepthPass() const override;

        void Use() const override;

        void UseDirectionalShadows() const override;

        void UsePointSpotShadows() const override;

        std::string GetType() const override
        {
            return TypeName;
        }

        MATERIAL_SERIALIZATION_METHODS_DECLARATIONS
    };

} // Models
