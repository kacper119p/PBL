#include "WaterMaterial.h"
#include "GLFW/glfw3.h"
#include "Serialization/SerializationUtility.h"
#include "Shaders/ShaderManager.h"
#include "Shaders/ShaderSourceFiles.h"

namespace Materials
{
    const Shaders::Shader WaterMaterial::DepthPass = Shaders::ShaderManager::GetShader(
            Shaders::ShaderSourceFiles("./res/shaders/DefaultDepth/DefaultDepth.vert", nullptr,
                                       "./res/shaders/DefaultDepth/DefaultDepth.frag"));
    const Shaders::Shader WaterMaterial::MainPass = Shaders::ShaderManager::GetShader(
            Shaders::ShaderSourceFiles("./res/shaders/Water/Water.vert", nullptr, "./res/shaders/Water/Water.frag"));
    const Shaders::Shader WaterMaterial::DirectionalShadowPass = Shaders::ShaderManager::GetShader(
            Shaders::ShaderSourceFiles("./res/shaders/Common/BasicShadowPass/DirectionalLight.vert", nullptr,
                                       "./res/shaders/Common/BasicShadowPass/DirectionalLight.frag"));
    const Shaders::Shader WaterMaterial::PointSpotShadowPass = Shaders::ShaderManager::GetShader(
            Shaders::ShaderSourceFiles("./res/shaders/Common/BasicShadowPass/PointSpotLight.vert",
                                       "./res/shaders/Common/BasicShadowPass/PointSpotLight.geom",
                                       "./res/shaders/Common/BasicShadowPass/PointSpotLight.frag"));

    WaterMaterial::WaterMaterial(const unsigned int NormalMap0, const unsigned int NormalMap1, const glm::vec3& Color,
                                 const glm::vec2& Tiling0, const glm::vec2& Tiling1, const glm::vec2& Velocity0,
                                 const glm::vec2& Velocity1, const float Roughness, const float Metallic) :
        Material(DepthPass, MainPass, DirectionalShadowPass, PointSpotShadowPass),
        NormalMap0(Engine::Texture(NormalMap0)), NormalMap1(Engine::Texture(NormalMap1)),
        Color(Vector3MaterialProperty("Color", MainPass, Color)),
        Tiling0(Vector2MaterialProperty("Tiling0", MainPass, Tiling0)),
        Tiling1(Vector2MaterialProperty("Tiling1", MainPass, Tiling1)),
        Velocity0(Vector2MaterialProperty("Velocity0", MainPass, Velocity0)),
        Velocity1("Velocity1", MainPass, Velocity1), Roughness(FloatMaterialProperty("Roughness", MainPass, Roughness)),
        Metallic(FloatMaterialProperty("Metallic", MainPass, Metallic)),
        TimeLocation(MainPass.GetUniformLocation("Time"))
    {
    }

    WaterMaterial::WaterMaterial() :
        Material(DepthPass, MainPass, DirectionalShadowPass, PointSpotShadowPass), NormalMap0(Engine::Texture()),
        NormalMap1(Engine::Texture()), Color(Vector3MaterialProperty("Color", MainPass)),
        Tiling0(Vector2MaterialProperty("Tiling0", MainPass)), Tiling1(Vector2MaterialProperty("Tiling1", MainPass)),
        Velocity0(Vector2MaterialProperty("Velocity0", MainPass)), Velocity1("Velocity1", MainPass),
        Roughness(FloatMaterialProperty("Roughness", MainPass)), Metallic(FloatMaterialProperty("Metallic", MainPass)),
        TimeLocation(MainPass.GetUniformLocation("Time"))
    {
    }

    void WaterMaterial::UseDepthPass() const
    {
        GetDepthPass().Use();
    }

    void WaterMaterial::Use() const
    {
        GetMainPass().Use();
        Color.Bind();
        Roughness.Bind();
        Metallic.Bind();
        Tiling0.Bind();
        Tiling1.Bind();
        Velocity0.Bind();
        Velocity1.Bind();

        Shaders::Shader::SetUniform(TimeLocation, static_cast<float>(glfwGetTime()));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, NormalMap0.GetId());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, NormalMap1.GetId());
    }

    void WaterMaterial::UseDirectionalShadows() const
    {
        GetDirectionalShadowPass().Use();
    }

    void WaterMaterial::UsePointSpotShadows() const
    {
        GetPointSpotShadowPass().Use();
    }

    rapidjson::Value WaterMaterial::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_MATERIAL_SERIALIZATION
        SERIALIZE_PROPERTY(NormalMap0)
        SERIALIZE_PROPERTY(NormalMap1)
        SERIALIZE_PROPERTY(Color)
        SERIALIZE_PROPERTY(Tiling0)
        SERIALIZE_PROPERTY(Tiling1)
        SERIALIZE_PROPERTY(Velocity0)
        SERIALIZE_PROPERTY(Velocity1)
        SERIALIZE_PROPERTY(Roughness)
        SERIALIZE_PROPERTY(Metallic)
        END_MATERIAL_SERIALIZATION
    }

    void WaterMaterial::Deserialize(const rapidjson::Value& Object)
    {
        START_MATERIAL_DESERIALIZATION
        DESERIALIZE_PROPERTY(NormalMap0)
        DESERIALIZE_PROPERTY(NormalMap1)
        DESERIALIZE_PROPERTY(Color)
        DESERIALIZE_PROPERTY(Tiling0)
        DESERIALIZE_PROPERTY(Tiling1)
        DESERIALIZE_PROPERTY(Velocity0)
        DESERIALIZE_PROPERTY(Velocity1)
        DESERIALIZE_PROPERTY(Roughness)
        DESERIALIZE_PROPERTY(Metallic)
        END_MATERIAL_DESERIALIZATION
    }
} // Models
