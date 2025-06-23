#include "VacuumIndicatorMaterial.h"

#include "Engine/EngineObjects/LightManager.h"
#include "Serialization/SerializationUtility.h"
#include "Shaders/ShaderManager.h"
#include "Shaders/ShaderSourceFiles.h"

#if EDITOR
#include "imgui.h"
#include <filesystem>
namespace fs = std::filesystem;
#include "Engine/Textures/TextureManager.h"
#endif

namespace Materials
{
    Shaders::Shader VacuumIndicatorMaterial::DepthPass;
    Shaders::Shader VacuumIndicatorMaterial::MainPass;
    Shaders::Shader VacuumIndicatorMaterial::DirectionalShadowPass;
    Shaders::Shader VacuumIndicatorMaterial::PointSpotShadowPass;

    VacuumIndicatorMaterial::VacuumIndicatorMaterial(const glm::vec3& EmissiveColor, const float Fill) :
        Material(DepthPass, MainPass, DirectionalShadowPass, PointSpotShadowPass),
        EmissiveColor(Vector3MaterialProperty("Color", MainPass, EmissiveColor)),
        Fill(FloatMaterialProperty("Fill", MainPass, Fill))
    {
    }

    VacuumIndicatorMaterial::VacuumIndicatorMaterial():
        Material(DepthPass, MainPass, DirectionalShadowPass, PointSpotShadowPass),
        EmissiveColor(Vector3MaterialProperty("Color", MainPass)),
        Fill(FloatMaterialProperty("Fill", MainPass))
    {
    }


    void VacuumIndicatorMaterial::Initialize()
    {
        DepthPass = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/DefaultDepth/DefaultDepthAnimated.vert", nullptr,
                "./res/shaders/DefaultDepth/DefaultDepth.frag"));
        MainPass = Shaders::ShaderManager::GetShader(
                Shaders::ShaderSourceFiles("./res/shaders/VacuumIndicator/VacuumIndicator.vert", nullptr,
                                           "./res/shaders/VacuumIndicator/VacuumIndicator.frag"));
        DirectionalShadowPass = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/PBR/PBRAnimated.vert", nullptr,
                "./res/shaders/Common/BasicShadowPass/DirectionalLight.frag"));
        PointSpotShadowPass = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/PBR/PBRAnimated.vert",
                "./res/shaders/Common/BasicShadowPass/PointSpotLight.geom",
                "./res/shaders/Common/BasicShadowPass/PointSpotLight.frag"));
    }

    void VacuumIndicatorMaterial::UseDepthPass() const
    {
        GetDepthPass().Use();
    }

    void VacuumIndicatorMaterial::Use() const
    {
        GetMainPass().Use();

        EmissiveColor.Bind();
        Fill.Bind();
    }

    void VacuumIndicatorMaterial::UseDirectionalShadows() const
    {
        GetDirectionalShadowPass().Use();
    }

    void VacuumIndicatorMaterial::UsePointSpotShadows() const
    {
        GetPointSpotShadowPass().Use();
    }

#if EDITOR
    void VacuumIndicatorMaterial::DrawImGui()
    {
        glm::vec3 emissiveColor = EmissiveColor.GetValue();
        if (ImGui::ColorEdit3("Emissive Color", glm::value_ptr(emissiveColor)))
        {
            EmissiveColor.SetValue(emissiveColor);
        }
    }
#endif

    rapidjson::Value VacuumIndicatorMaterial::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_MATERIAL_SERIALIZATION
        SERIALIZE_PROPERTY(EmissiveColor);
        END_MATERIAL_SERIALIZATION
    }

    void VacuumIndicatorMaterial::Deserialize(const rapidjson::Value& Object)
    {
        START_MATERIAL_DESERIALIZATION
        DESERIALIZE_PROPERTY(EmissiveColor);
        END_MATERIAL_DESERIALIZATION
    }
}
