#include "SkyboxMaterial.h"

#include "Serialization/SerializationUtility.h"
#include "Shaders/ShaderManager.h"
#include "Shaders/ShaderSourceFiles.h"
#include "Engine/Textures/TextureManager.h"

#if EDITOR
#include <filesystem>
#include "imgui.h"
#endif

namespace Materials
{
    Shaders::Shader SkyboxMaterial::DepthPass;
    Shaders::Shader SkyboxMaterial::MainPass;
    Shaders::Shader SkyboxMaterial::DirectionalShadowPass;
    Shaders::Shader SkyboxMaterial::PointSpotShadowPass;

    SkyboxMaterial::SkyboxMaterial(const Engine::Texture Texture) :
        Material(DepthPass, MainPass, DirectionalShadowPass, PointSpotShadowPass),
        Texture(TextureMaterialProperty("Texture", MainPass, Texture))
    {
    }

    SkyboxMaterial::SkyboxMaterial():
        Material(DepthPass, MainPass, DirectionalShadowPass, PointSpotShadowPass),
        Texture(TextureMaterialProperty("Texture", MainPass))
    {
    }

    void SkyboxMaterial::Initialize()
    {
        DepthPass = Shaders::ShaderManager::GetShader(
                Shaders::ShaderSourceFiles("./res/shaders/Skybox/Skybox.vert", nullptr,
                                           "./res/shaders/Skybox/Skybox.frag"));
        MainPass = Shaders::ShaderManager::GetShader(
                Shaders::ShaderSourceFiles("./res/shaders/Skybox/Skybox.vert", nullptr,
                                           "./res/shaders/Skybox/Skybox.frag"));
        DirectionalShadowPass = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/Skybox/SkyboxDirectionalLightShadow.vert", nullptr,
                "./res/shaders/Skybox/SkyboxDirectionalLightShadow.frag"));
        PointSpotShadowPass = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/Skybox/SkyboxDirectionalLightShadow.vert", nullptr,
                "./res/shaders/Skybox/SkyboxDirectionalLightShadow.frag"));
    }

    void SkyboxMaterial::UseDepthPass() const
    {
        GetDepthPass().Use();
    }

    void SkyboxMaterial::Use() const
    {
        GetMainPass().Use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, Texture.GetValue().GetId());
    }

    void SkyboxMaterial::UseDirectionalShadows() const
    {
        GetDirectionalShadowPass().Use();
    }

    void SkyboxMaterial::UsePointSpotShadows() const
    {
        GetPointSpotShadowPass().Use();
    }
#if EDITOR
    void SkyboxMaterial::DrawImGui()
    {
        static bool showBaseTexPopup = false;
        static std::vector<std::string> availableTextures;
        std::string texturePath = std::filesystem::absolute("./res/textures").string();
        static bool scanned = false;
        if (!scanned)
        {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(texturePath))
            {
                if (entry.is_regular_file() && entry.path().extension() == ".hdr")
                {
                    std::string relPath = entry.path().lexically_normal().string();
                    std::replace(relPath.begin(), relPath.end(), '\\', '/');
                    size_t pos = relPath.find("/res/");
                    if (pos != std::string::npos)
                        relPath = "." + relPath.substr(pos);
                    availableTextures.emplace_back(relPath);
                }
            }
            scanned = true;
        }
        std::string baseMapPath = Texture.GetValue().GetId() != 0
                                      ? Engine::TextureManager::GetTexturePath(Texture.GetValue())
                                      : "None";
        ImGui::Separator();
        ImGui::Text("Base Map:");
        ImGui::Selectable(baseMapPath.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick);
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH"))
            {
                const char* droppedPath = static_cast<const char*>(payload->Data);
                if (std::filesystem::path(droppedPath).extension() == ".hdr")
                {
                    Texture.SetValue(Engine::TextureManager::GetTexture(droppedPath));
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
                    Texture.SetValue(Engine::TextureManager::GetTexture(path.c_str()));
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                ImGui::TextDisabled("(%s)", path.c_str());
            }
            ImGui::EndPopup();
        }

    }
#endif
    rapidjson::Value SkyboxMaterial::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_MATERIAL_SERIALIZATION
        SERIALIZE_PROPERTY(Texture);
        END_MATERIAL_SERIALIZATION
    }

    void SkyboxMaterial::Deserialize(const rapidjson::Value& Object)
    {
        START_MATERIAL_DESERIALIZATION
        DESERIALIZE_PROPERTY(Texture);
        END_MATERIAL_DESERIALIZATION
    }
} // Models
