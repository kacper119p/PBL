#include "AnimatedPbrMaterial.h"

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
    Shaders::Shader AnimatedPbrMaterial::DepthPass;
    Shaders::Shader AnimatedPbrMaterial::MainPass;
    Shaders::Shader AnimatedPbrMaterial::DirectionalShadowPass;
    Shaders::Shader AnimatedPbrMaterial::PointSpotShadowPass;

    AnimatedPbrMaterial::AnimatedPbrMaterial(const Engine::Texture BaseMap, const Engine::Texture RoughnessMetallicMap,
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
        EmissiveColor(Vector3MaterialProperty("EmissiveColor", MainPass, EmissiveColor))
    {
    }

    AnimatedPbrMaterial::AnimatedPbrMaterial():
        Material(DepthPass, MainPass, DirectionalShadowPass, PointSpotShadowPass),
        BaseMap(TextureMaterialProperty("BaseMap", MainPass)),
        RoughnessMetallicMap(TextureMaterialProperty("RoughnessMetallicAmbientOcclusionMap", MainPass)),
        NormalMap(TextureMaterialProperty("NormalMap", MainPass)),
        EmissiveMap(TextureMaterialProperty("EmissiveMap", MainPass)),
        Roughness(FloatMaterialProperty("Roughness", MainPass)), Metallic(FloatMaterialProperty("Metallic", MainPass)),
        BaseColor(Vector3MaterialProperty("BaseColor", MainPass)),
        EmissiveColor(Vector3MaterialProperty("EmissiveColor", MainPass))
    {
    }


    void AnimatedPbrMaterial::Initialize()
    {
        DepthPass = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/DefaultDepth/DefaultDepthAnimated.vert", nullptr,
                "./res/shaders/DefaultDepth/DefaultDepth.frag"));
        MainPass = Shaders::ShaderManager::GetShader(
                Shaders::ShaderSourceFiles("./res/shaders/PBR/PBRAnimated.vert", nullptr,
                                           "./res/shaders/PBR/PBR.frag"));
        DirectionalShadowPass = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/PBR/PBRAnimated.vert", nullptr,
                "./res/shaders/Common/BasicShadowPass/DirectionalLight.frag"));
        PointSpotShadowPass = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/PBR/PBRAnimated.vert",
                "./res/shaders/Common/BasicShadowPass/PointSpotLight.geom",
                "./res/shaders/Common/BasicShadowPass/PointSpotLight.frag"));
    }

    void AnimatedPbrMaterial::UseDepthPass() const
    {
        GetDepthPass().Use();
    }

    void AnimatedPbrMaterial::Use() const
    {
        GetMainPass().Use();
        Engine::LightManager::GetInstance()->SetupLightsForRendering(MainPass);

        BaseColor.Bind();
        EmissiveColor.Bind();
        Roughness.Bind();
        Metallic.Bind();

        BaseMap.Bind();
        RoughnessMetallicMap.Bind();
        NormalMap.Bind();
        EmissiveMap.Bind();
    }

    void AnimatedPbrMaterial::UseDirectionalShadows() const
    {
        GetDirectionalShadowPass().Use();
    }

    void AnimatedPbrMaterial::UsePointSpotShadows() const
    {
        GetPointSpotShadowPass().Use();
    }

#if EDITOR
    void AnimatedPbrMaterial::DrawImGui()
    {
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
        static bool showBaseTexPopup = false;
        static bool showRMTexPopup = false;
        static bool showNormalTexPopup = false;
        static bool showEmissiveTexPopup = false;
        static std::vector<std::string> availableTextures;
        std::string texturePath = std::filesystem::absolute("./res/textures").string();
        static bool scanned = false;

        if (!scanned)
        {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(texturePath))
            {
                if (entry.is_regular_file() && entry.path().extension() == ".png")
                    availableTextures.emplace_back(entry.path().string());
            }
            scanned = true;
        }

        std::string baseMapPath = BaseMap.GetValue().GetId() != 0
                                      ? Engine::TextureManager::GetTexturePath(BaseMap.GetValue())
                                      : "None";
        std::string roughnessMetallicMapPath = RoughnessMetallicMap.GetValue().GetId() != 0
                                                   ? Engine::TextureManager::GetTexturePath(
                                                           RoughnessMetallicMap.GetValue())
                                                   : "None";
        std::string normalMapPath = NormalMap.GetValue().GetId() != 0
                                        ? Engine::TextureManager::GetTexturePath(NormalMap.GetValue())
                                        : "None";
        std::string emissiveMapPath =
                EmissiveMap.GetValue().GetId() != 0
                    ? Engine::TextureManager::GetTexturePath(EmissiveMap.GetValue())
                    : "None";
        ImGui::Separator();
        ImGui::Text("Base Map:");
        ImGui::Selectable(baseMapPath.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick);
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH"))
            {
                const char* droppedPath = static_cast<const char*>(payload->Data);
                if (std::filesystem::path(droppedPath).extension() == ".png")
                {
                    BaseMap.SetValue(Engine::TextureManager::GetTexture(droppedPath));
                }
            }
            ImGui::EndDragDropTarget();
        }
        if (ImGui::IsItemClicked())
            showBaseTexPopup = true;

        if (showBaseTexPopup)
        {
            ImGui::OpenPopup("Base Map Picker");
            showBaseTexPopup = false;
        }
        if (ImGui::BeginPopup("Base Map Picker"))
        {
            for (const auto& path : availableTextures)
            {
                std::filesystem::path fsPath(path);
                std::string displayName = std::filesystem::relative(fsPath, texturePath).string();
                if (ImGui::Selectable(displayName.c_str()))
                {
                    BaseMap.SetValue(Engine::TextureManager::GetTexture(path.c_str()));
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                ImGui::TextDisabled("(%s)", path.c_str());
            }
            ImGui::EndPopup();
        }
        ImGui::Separator();
        ImGui::Text("Roughness Metallic Map:");
        ImGui::Selectable(roughnessMetallicMapPath.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick);
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH"))
            {
                const char* droppedPath = static_cast<const char*>(payload->Data);
                if (std::filesystem::path(droppedPath).extension() == ".png")
                {
                    RoughnessMetallicMap.SetValue(Engine::TextureManager::GetTexture(droppedPath));
                }
            }
            ImGui::EndDragDropTarget();
        }
        if (ImGui::IsItemClicked())
            showRMTexPopup = true;

        if (showRMTexPopup)
        {
            ImGui::OpenPopup("Rougness Metallic Map Picker");
            showRMTexPopup = false;
        }

        if (ImGui::BeginPopup("Rougness Metallic Map Picker"))
        {
            for (const auto& path : availableTextures)
            {
                std::filesystem::path fsPath(path);
                std::string displayName = std::filesystem::relative(fsPath, texturePath).string();
                if (ImGui::Selectable(displayName.c_str()))
                {
                    RoughnessMetallicMap.SetValue(Engine::TextureManager::GetTexture(path.c_str()));
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                ImGui::TextDisabled("(%s)", path.c_str());
            }
            ImGui::EndPopup();
        }
        ImGui::Separator();
        ImGui::Text("Normal Map:");
        ImGui::Selectable(normalMapPath.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick);
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH"))
            {
                const char* droppedPath = static_cast<const char*>(payload->Data);
                if (std::filesystem::path(droppedPath).extension() == ".png")
                {
                    NormalMap.SetValue(Engine::TextureManager::GetTexture(droppedPath));
                }
            }
            ImGui::EndDragDropTarget();
        }
        if (ImGui::IsItemClicked())
            showNormalTexPopup = true;
        if (showNormalTexPopup)
        {
            ImGui::OpenPopup("Normal Map Picker");
            showNormalTexPopup = false;
        }
        if (ImGui::BeginPopup("Normal Map Picker"))
        {
            for (const auto& path : availableTextures)
            {
                std::filesystem::path fsPath(path);
                std::string displayName = std::filesystem::relative(fsPath, texturePath).string();
                if (ImGui::Selectable(displayName.c_str()))
                {
                    NormalMap.SetValue(Engine::TextureManager::GetTexture(path.c_str()));
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                ImGui::TextDisabled("(%s)", path.c_str());
            }
            ImGui::EndPopup();
        }
        ImGui::Text("Emissive Map:");
        ImGui::Selectable(emissiveMapPath.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick);
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH"))
            {
                const char* droppedPath = static_cast<const char*>(payload->Data);
                if (std::filesystem::path(droppedPath).extension() == ".png")
                {
                    EmissiveMap.SetValue(Engine::TextureManager::GetTexture(droppedPath));
                }
            }
            ImGui::EndDragDropTarget();
        }
        if (ImGui::IsItemClicked())
            showEmissiveTexPopup = true;
        if (showEmissiveTexPopup)
        {
            ImGui::OpenPopup("Emissive Map Picker");
            showEmissiveTexPopup = false;
        }
        if (ImGui::BeginPopup("Emissive Map Picker"))
        {
            for (const auto& path : availableTextures)
            {
                std::filesystem::path fsPath(path);
                std::string displayName = std::filesystem::relative(fsPath, texturePath).string();
                if (ImGui::Selectable(displayName.c_str()))
                {
                    EmissiveMap.SetValue(Engine::TextureManager::GetTexture(path.c_str()));
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                ImGui::TextDisabled("(%s)", path.c_str());
            }
            ImGui::EndPopup();
        }
    }
#endif

    rapidjson::Value AnimatedPbrMaterial::Serialize(rapidjson::Document::AllocatorType& Allocator) const
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

    void AnimatedPbrMaterial::Deserialize(const rapidjson::Value& Object)
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
