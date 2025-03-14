#pragma once

#include "Shaders/Shader.h"

namespace Materials
{
    /**
     * @brief Base class for all materials.
     */
    class Material
    {
    private:
        const Shaders::Shader DepthPass;
        const Shaders::Shader MainPass;
        const Shaders::Shader DirectionalShadowPass;
        const Shaders::Shader PointSpotShadowPass;

    public:
        Material(const Shaders::Shader& DepthPass, const Shaders::Shader& Shader,
                 const Shaders::Shader& DirectionalShadowPass, const Shaders::Shader& PointSpotShadowPass) :
            DepthPass(DepthPass), MainPass(Shader), DirectionalShadowPass(DirectionalShadowPass),
            PointSpotShadowPass(PointSpotShadowPass)
        {
        }

    public:
        inline virtual ~Material() = default;

    public:
        /**
         * @brief Returns depth pass shader used by this material.
         */
        [[nodiscard]] const Shaders::Shader& GetDepthPass() const
        {
            return DepthPass;
        }

        /**
         * @brief Returns main pass shader used by this material.
         */
        [[nodiscard]] const Shaders::Shader& GetMainPass() const
        {
            return MainPass;
        }

        /**
         * @brief Returns point light shadow pass shader used by this material.
         */
        [[nodiscard]] const Shaders::Shader& GetPointSpotShadowPass() const
        {
            return PointSpotShadowPass;
        }

        /**
         * @brief Returns directional light shadow pass shader used by this material.
         */
        [[nodiscard]] const Shaders::Shader& GetDirectionalShadowPass() const
        {
            return DirectionalShadowPass;
        }

        /**
         * @brief Binds depth pass and its uniforms used by this material to be used in the next draw call.
         */
        virtual void UseDepthPass() const = 0;

        /**
         * @brief Binds main pass and its uniforms used by this material to be used in the next draw call.
         */
        virtual void Use() const = 0;

        /**
         * @brief Binds point light shadow pass and its uniforms used by this material to be used in the next draw call.
         */
        virtual void UseDirectionalShadows() const = 0;

        /**
         * @brief Binds directional light shadow pass and its uniforms used by this material to be used in the next draw call.
         */
        virtual void UsePointSpotShadows() const = 0;
    };
}
