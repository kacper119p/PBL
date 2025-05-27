#include "FloorMaterial.h"

#include "Engine/Components/BloodSystem/BloodManager.h"
#include "Serialization/SerializationUtility.h"
#include "Shaders/ShaderManager.h"
#include "Shaders/ShaderSourceFiles.h"

namespace Materials
{
    Shaders::Shader FloorMaterial::DepthPass;
    Shaders::Shader FloorMaterial::MainPass;
    Shaders::Shader FloorMaterial::DirectionalShadowPass;
    Shaders::Shader FloorMaterial::PointSpotShadowPass;

    FloorMaterial::FloorMaterial(const Engine::Texture BaseMap, const Engine::Texture RoughnessMetallicMap,
                                 const Engine::Texture NormalMap, const Engine::Texture EmissiveMap,
                                 const glm::vec3& BaseColor, const float Roughness, const float Metallic,
                                 const glm::vec3& EmissiveColor) :
        Material(DepthPass, MainPass, DirectionalShadowPass, PointSpotShadowPass),
        BaseMap(TextureMaterialProperty("BaseMap", MainPass, BaseMap)),
        RoughnessMetallicMap(TextureMaterialProperty("RoughnessMetallicAmbientOcclusionMap", MainPass,
                                                     RoughnessMetallicMap)),
        NormalMap(TextureMaterialProperty("NormalMap", MainPass, NormalMap)),
        EmissiveMap(TextureMaterialProperty("EmissiveMap", MainPass, EmissiveMap)),
        Roughness(FloatMaterialProperty("Roughness", MainPass, Roughness)),
        Metallic(FloatMaterialProperty("Metallic", MainPass, Metallic)),
        BaseColor(Vector3MaterialProperty("BaseColor", MainPass, BaseColor)),
        EmissiveColor(Vector3MaterialProperty("EmissiveColor", MainPass, EmissiveColor)),
        BloodMaskViewProjectionLocation(MainPass.GetUniformLocation("BloodMaskViewProjection"))
    {
    }

    FloorMaterial::FloorMaterial():
        Material(DepthPass, MainPass, DirectionalShadowPass, PointSpotShadowPass),
        BaseMap(TextureMaterialProperty("BaseMap", MainPass)),
        RoughnessMetallicMap(TextureMaterialProperty("RoughnessMetallicAmbientOcclusionMap", MainPass)),
        NormalMap(TextureMaterialProperty("NormalMap", MainPass)),
        EmissiveMap(TextureMaterialProperty("EmissiveMap", MainPass)),
        Roughness(FloatMaterialProperty("Roughness", MainPass)), Metallic(FloatMaterialProperty("Metallic", MainPass)),
        BaseColor(Vector3MaterialProperty("BaseColor", MainPass)),
        EmissiveColor(Vector3MaterialProperty("EmissiveColor", MainPass)),
        BloodMaskViewProjectionLocation(MainPass.GetUniformLocation("BloodMaskViewProjection"))
    {
    }

    void FloorMaterial::Initialize()
    {
        DepthPass = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/DefaultDepth/DefaultDepth.vert", nullptr,
                "./res/shaders/DefaultDepth/DefaultDepth.frag"));
        MainPass = Shaders::ShaderManager::GetShader(
                Shaders::ShaderSourceFiles("./res/shaders/Floor/Floor.vert", nullptr,
                                           "./res/shaders/Floor/Floor.frag"));
        DirectionalShadowPass = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/Common/BasicShadowPass/DirectionalLight.vert", nullptr,
                "./res/shaders/Common/BasicShadowPass/DirectionalLight.frag"));
        PointSpotShadowPass = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/Common/BasicShadowPass/PointSpotLight.vert",
                "./res/shaders/Common/BasicShadowPass/PointSpotLight.geom",
                "./res/shaders/Common/BasicShadowPass/PointSpotLight.frag"));
    }

    void FloorMaterial::UseDepthPass() const
    {
        GetDepthPass().Use();
    }

    void FloorMaterial::Use() const
    {
        GetMainPass().Use();

        BaseColor.Bind();
        EmissiveColor.Bind();
        Roughness.Bind();
        Metallic.Bind();

        BaseMap.Bind();
        RoughnessMetallicMap.Bind();
        NormalMap.Bind();
        EmissiveMap.Bind();

        const Engine::BloodManager* const bloodManager = Engine::BloodManager::GetCurrent();

        if (bloodManager == nullptr)
        {
            return;
        }

        glActiveTexture(GL_TEXTURE0 + BloodMaskLocation);
        glBindTexture(GL_TEXTURE_2D, bloodManager->GetMaskId());
        MainPass.SetUniform(BloodMaskViewProjectionLocation, bloodManager->GetMaskSpaceTransform());
    }

    void FloorMaterial::UseDirectionalShadows() const
    {
        GetDirectionalShadowPass().Use();
    }

    void FloorMaterial::UsePointSpotShadows() const
    {
        GetPointSpotShadowPass().Use();
    }

#if EDITOR
    void FloorMaterial::DrawImGui()
    {
        // Draw scalar and vector parameters
        glm::vec3 baseColor = BaseColor.GetValue();
        if (ImGui::ColorEdit3("Base Color", &baseColor.x))
        {
            BaseColor.SetValue(baseColor);
        }

        glm::vec3 emissiveColor = EmissiveColor.GetValue();
        if (ImGui::ColorEdit3("Emissive Color", &emissiveColor.x))
        {
            EmissiveColor.SetValue(emissiveColor);
        }

        float roughness = Roughness.GetValue();
        if (ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f))
        {
            Roughness.SetValue(roughness);
        }

        float metallic = Metallic.GetValue();
        if (ImGui::SliderFloat("Metallic", &metallic, 0.0f, 1.0f))
        {
            Metallic.SetValue(metallic);
        }

        // Texture browsing support
        static std::vector<std::string> availableTextures;
        static bool scanned = false;
        std::string texturePath = std::filesystem::relative("./res/textures").string();

        EditorReadMaterial(availableTextures, scanned, texturePath);

        // Draw texture selectors using base helper
        static bool showBaseTexPopup = false;
        static bool showRMTexPopup = false;
        static bool showNormalTexPopup = false;
        static bool showEmissiveTexPopup = false;

        EditorSetMaterial("Base Map", BaseMap, showBaseTexPopup, availableTextures, texturePath, "Base Map Picker");
        ImGui::Separator();

        EditorSetMaterial("Roughness Metallic Map", RoughnessMetallicMap, showRMTexPopup, availableTextures,
                          texturePath, "Roughness Metallic Map Picker");
        ImGui::Separator();

        EditorSetMaterial("Normal Map", NormalMap, showNormalTexPopup, availableTextures, texturePath,
                          "Normal Map Picker");
        ImGui::Separator();

        EditorSetMaterial("Emissive Map", EmissiveMap, showEmissiveTexPopup, availableTextures, texturePath,
                          "Emissive Map Picker");
    }
#endif


    rapidjson::Value FloorMaterial::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_MATERIAL_SERIALIZATION
        SERIALIZE_PROPERTY(BaseMap);
        SERIALIZE_PROPERTY(RoughnessMetallicMap);
        SERIALIZE_PROPERTY(NormalMap);
        SERIALIZE_PROPERTY(EmissiveMap);
        SERIALIZE_PROPERTY(Roughness);
        SERIALIZE_PROPERTY(Metallic);
        SERIALIZE_PROPERTY(BaseColor);
        SERIALIZE_PROPERTY(EmissiveColor);
        END_MATERIAL_SERIALIZATION
    }

    void FloorMaterial::Deserialize(const rapidjson::Value& Object)
    {
        START_MATERIAL_DESERIALIZATION
        DESERIALIZE_PROPERTY(BaseMap);
        DESERIALIZE_PROPERTY(RoughnessMetallicMap);
        DESERIALIZE_PROPERTY(NormalMap);
        DESERIALIZE_PROPERTY(EmissiveMap);
        DESERIALIZE_PROPERTY(Roughness);
        DESERIALIZE_PROPERTY(Metallic);
        DESERIALIZE_PROPERTY(BaseColor);
        DESERIALIZE_PROPERTY(EmissiveColor);
        END_MATERIAL_DESERIALIZATION
    }
}
