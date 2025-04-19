#pragma once

#include <string>

#include "Materials/MaterialRaii.h" //Used in SERIALIZATION_EXPORT_MATERIAL
#include "rapidjson/document.h"
#include "Shaders/Shader.h"
#include "imgui.h"

#define SERIALIZATION_EXPORT_MATERIAL(__CLASS__)\
public:\
    static inline const std::string TypeName =std::string(#__CLASS__);\
    \
    rapidjson::Value Serialize(rapidjson::Document::AllocatorType& Allocator) const override;\
    \
    void Deserialize(const rapidjson::Value& Object) override;\
    \
    [[nodiscard]] std::string GetType() const override\
    {\
        return TypeName;\
    }\
    \
    static void Initialize();\
    \
private:\
    static inline const MaterialRaii<__CLASS__> RaiiHandle =  MaterialRaii<__CLASS__>(#__CLASS__);

#define START_MATERIAL_SERIALIZATION rapidjson::Value object(rapidjson::kObjectType);\
                                     object.AddMember("type", Serialization::Serialize(GetType(), Allocator), Allocator);

#define END_MATERIAL_SERIALIZATION  return object;

#define START_MATERIAL_DESERIALIZATION

#define END_MATERIAL_DESERIALIZATION

#define SERIALIZE_PROPERTY(__NAME__) object.AddMember(#__NAME__, Serialization::Serialize(__NAME__, Allocator), Allocator);

#define DESERIALIZE_PROPERTY(__NAME__) Serialization::Deserialize(Object, #__NAME__, __NAME__);

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
         * @brief Returns class name of this material.
         */
        [[nodiscard]] virtual std::string GetType() const = 0;

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

        /**
         * @brief Saves this material's properties to a JSON.
         * @param Allocator An allocator to be used.
         * @return Serialized data.
         */
        virtual rapidjson::Value Serialize(rapidjson::Document::AllocatorType& Allocator) const = 0;

        /**
         * @brief Sets this material's properties from JSON.
         * @param Value Data to load properties from.
         */
        virtual void Deserialize(const rapidjson::Value& Value) = 0;

        virtual void DrawImGui() {};
    };
}
