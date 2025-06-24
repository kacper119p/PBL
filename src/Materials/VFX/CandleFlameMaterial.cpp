#include "CandleFlameMaterial.h"

#include "Engine/Textures/TextureManager.h"
#include "GLFW/glfw3.h"
#include "Serialization/SerializationUtility.h"
#include "Shaders/ShaderManager.h"
#include "Shaders/ShaderSourceFiles.h"

#if EDITOR
#include "imgui.h"
#endif
namespace Materials
{
    Shaders::Shader CandleFlameMaterial::Shader;

    CandleFlameMaterial::CandleFlameMaterial() :
        Material(Shaders::Shader(), Shader, Shaders::Shader(), Shaders::Shader(), true),
        SpriteProperty(TextureMaterialProperty("Sprite", Shader,
                                               Engine::TextureManager::GetTexture(
                                                       "res/textures/VFX/Candle/Candle.dds"))),
        NoiseProperty("NoiseParameters", Shader, glm::vec4(2.0f, -2.0f, 4.0f, 3.0f)),
        ColorProperty(Vector3MaterialProperty("Color", Shader, glm::vec3(63.43752f, 4.76168f, 0.74808f))),
        NoiseStrengthProperty("NoiseStrength", Shader, glm::vec2(0.2f, 0.15f)),
        TimeLocation(Shader.GetUniformLocation("Time"))
    {
    }

    void CandleFlameMaterial::Initialize()
    {
        Shader = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/VFX/CandleFlame/CandleFlame.vert",
                nullptr,
                "./res/shaders/VFX/CandleFlame/CandleFlame.frag"));
    }


    void CandleFlameMaterial::UseDepthPass() const
    {
    }

    void CandleFlameMaterial::Use() const
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        GetMainPass().Use();
        SpriteProperty.Bind();
        ColorProperty.Bind();
        NoiseProperty.Bind();
        NoiseStrengthProperty.Bind();
        Shaders::Shader::SetUniform(TimeLocation, static_cast<float>(glfwGetTime()));
    }

    void CandleFlameMaterial::UseDirectionalShadows() const
    {
    }

    void CandleFlameMaterial::UsePointSpotShadows() const
    {
    }
#if EDITOR
    void CandleFlameMaterial::DrawImGui()
    {
        glm::vec3 color = ColorProperty.GetValue();
        if (ImGui::ColorPicker3("Color", glm::value_ptr(color)))
        {
            ColorProperty.SetValue(color);
        }

        glm::vec4 noiseParameters = NoiseProperty.GetValue();

        if (ImGui::InputFloat2("Noise Velocity", glm::value_ptr(noiseParameters)))
        {
            NoiseProperty.SetValue(noiseParameters);
        }

        if (ImGui::InputFloat2("Noise Scale", &noiseParameters.z))
        {
            NoiseProperty.SetValue(noiseParameters);
        }

        glm::vec2 noiseStrength = NoiseStrengthProperty.GetValue();
        if (ImGui::InputFloat2("Noise Strength", glm::value_ptr(noiseStrength)))
        {
            NoiseStrengthProperty.SetValue(noiseStrength);
        }
    }
#endif
    rapidjson::Value CandleFlameMaterial::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_MATERIAL_SERIALIZATION
        SERIALIZE_PROPERTY(NoiseProperty)
        SERIALIZE_PROPERTY(ColorProperty)
        SERIALIZE_PROPERTY(NoiseStrengthProperty)
        END_MATERIAL_SERIALIZATION
    }

    void CandleFlameMaterial::Deserialize(const rapidjson::Value& Object)
    {
        START_MATERIAL_DESERIALIZATION
        DESERIALIZE_PROPERTY(NoiseProperty)
        DESERIALIZE_PROPERTY(ColorProperty)
        DESERIALIZE_PROPERTY(NoiseStrengthProperty)
        END_MATERIAL_DESERIALIZATION
    }
} // Models
