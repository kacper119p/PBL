#include "PbrMaterial.h"

#include "Serialization/SerializationUtility.h"
#include "Shaders/ShaderManager.h"
#include "Engine/Textures/TextureManager.h"
#include <filesystem>

#if EDITOR
#include "imgui.h"
#endif

namespace Materials
{
    Shaders::Shader PbrMaterial::DepthPass;
    Shaders::Shader PbrMaterial::MainPass;
    Shaders::Shader PbrMaterial::DirectionalShadowPass;
    Shaders::Shader PbrMaterial::PointSpotShadowPass;

    PbrMaterial::PbrMaterial(const Engine::Texture BaseMap, const Engine::Texture RoughnessMetallicMap,
                             const Engine::Texture NormalMap, const Engine::Texture EmissiveMap,
                             const glm::vec3& BaseColor, const float Roughness, const float Metallic,
                             const glm::vec3& EmissiveColor) :
        Material(DepthPass, MainPass, DirectionalShadowPass, PointSpotShadowPass), BaseMap(Engine::Texture(BaseMap)),
        RoughnessMetallicMap(Engine::Texture(RoughnessMetallicMap)), NormalMap(Engine::Texture(NormalMap)),
        EmissiveMap(Engine::Texture(EmissiveMap)), Roughness(FloatMaterialProperty("Roughness", MainPass, Roughness)),
        Metallic(FloatMaterialProperty("Metallic", MainPass, Metallic)),
        BaseColor(Vector3MaterialProperty("BaseColor", MainPass, BaseColor)),
        EmissiveColor(Vector3MaterialProperty("EmissiveColor", MainPass, EmissiveColor))
    {
    }

    PbrMaterial::PbrMaterial():
        Material(DepthPass, MainPass, DirectionalShadowPass, PointSpotShadowPass), BaseMap(Engine::Texture()),
        RoughnessMetallicMap(Engine::Texture()), NormalMap(Engine::Texture()), EmissiveMap(Engine::Texture()),
        Roughness(FloatMaterialProperty("Roughness", MainPass)), Metallic(FloatMaterialProperty("Metallic", MainPass)),
        BaseColor(Vector3MaterialProperty("BaseColor", MainPass)),
        EmissiveColor(Vector3MaterialProperty("EmissiveColor", MainPass))
    {
    }

    void PbrMaterial::Initialize()
    {
        DepthPass = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/DefaultDepth/DefaultDepth.vert", nullptr,
                "./res/shaders/DefaultDepth/DefaultDepth.frag"));
        MainPass = Shaders::ShaderManager::GetShader(
                Shaders::ShaderSourceFiles("./res/shaders/PBR/PBR.vert", nullptr, "./res/shaders/PBR/PBR.frag"));
        DirectionalShadowPass = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/Common/BasicShadowPass/DirectionalLight.vert", nullptr,
                "./res/shaders/Common/BasicShadowPass/DirectionalLight.frag"));
        PointSpotShadowPass = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/Common/BasicShadowPass/PointSpotLight.vert",
                "./res/shaders/Common/BasicShadowPass/PointSpotLight.geom",
                "./res/shaders/Common/BasicShadowPass/PointSpotLight.frag"));
    }

    void PbrMaterial::UseDepthPass() const
    {
        GetDepthPass().Use();
    }

    void PbrMaterial::Use() const
    {
        GetMainPass().Use();
        BaseColor.Bind();
        EmissiveColor.Bind();
        Roughness.Bind();
        Metallic.Bind();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, BaseMap.GetId());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, RoughnessMetallicMap.GetId());
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, NormalMap.GetId());
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, EmissiveMap.GetId());
    }

    void PbrMaterial::UseDirectionalShadows() const
    {
        GetDirectionalShadowPass().Use();
    }

    void PbrMaterial::UsePointSpotShadows() const
    {
        GetPointSpotShadowPass().Use();
    }

#if EDITOR
    void PbrMaterial::DrawImGui()
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

        std::string baseMapPath = BaseMap.GetId() != 0 ? Engine::TextureManager::GetTexturePath(BaseMap) : "None";
        std::string roughnessMetallicMapPath = RoughnessMetallicMap.GetId() != 0
                                                   ? Engine::TextureManager::GetTexturePath(RoughnessMetallicMap)
                                                   : "None";
        std::string normalMapPath = NormalMap.GetId() != 0 ? Engine::TextureManager::GetTexturePath(NormalMap) : "None";
        std::string emissiveMapPath =
                EmissiveMap.GetId() != 0 ? Engine::TextureManager::GetTexturePath(EmissiveMap) : "None";
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
                    BaseMap = Engine::TextureManager::GetTexture(droppedPath);
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
                    BaseMap = Engine::TextureManager::GetTexture(path.c_str());
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
                    RoughnessMetallicMap = Engine::TextureManager::GetTexture(droppedPath);
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
                    RoughnessMetallicMap = Engine::TextureManager::GetTexture(path.c_str());
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
                    NormalMap = Engine::TextureManager::GetTexture(droppedPath);
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
                    NormalMap = Engine::TextureManager::GetTexture(path.c_str());
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
                    EmissiveMap = Engine::TextureManager::GetTexture(droppedPath);
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
                    EmissiveMap = Engine::TextureManager::GetTexture(path.c_str());
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                ImGui::TextDisabled("(%s)", path.c_str());
            }
            ImGui::EndPopup();
        }
    }
#endif


    rapidjson::Value PbrMaterial::Serialize(rapidjson::Document::AllocatorType& Allocator) const
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

    void PbrMaterial::Deserialize(const rapidjson::Value& Object)
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
