#include "SkyboxMaterial.h"

#include "Serialization/SerializationUtility.h"
#include "Shaders/ShaderManager.h"
#include "Shaders/ShaderSourceFiles.h"
#include "Engine/Textures/TextureManager.h"
#include <filesystem>

namespace Materials
{
    Shaders::Shader SkyboxMaterial::DepthPass;
    Shaders::Shader SkyboxMaterial::MainPass;
    Shaders::Shader SkyboxMaterial::DirectionalShadowPass;
    Shaders::Shader SkyboxMaterial::PointSpotShadowPass;

    SkyboxMaterial::SkyboxMaterial(const Engine::Texture Texture) :
        Material(DepthPass, MainPass, DirectionalShadowPass, PointSpotShadowPass), Texture(Texture)
    {
    }

    SkyboxMaterial::SkyboxMaterial():
        Material(DepthPass, MainPass, DirectionalShadowPass, PointSpotShadowPass), Texture(Engine::Texture())
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
        GetMainPass().SetTexture("Texture", 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, Texture.GetId());
    }

    void SkyboxMaterial::UseDirectionalShadows() const
    {
        GetDirectionalShadowPass().Use();
    }

    void SkyboxMaterial::UsePointSpotShadows() const
    {
        GetPointSpotShadowPass().Use(); }

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
                    availableTextures.emplace_back(entry.path().string());
            }
            scanned = true;
        }
        std::string baseMapPath = Texture.GetId() != 0 ? Engine::TextureManager::GetTexturePath(Texture) : "None";
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
                    Texture = Engine::TextureManager::GetTexture(droppedPath);
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
                    Texture = Engine::TextureManager::GetTexture(path.c_str());
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                ImGui::TextDisabled("(%s)", path.c_str());
            }
            ImGui::EndPopup();
        }
    
    }

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
