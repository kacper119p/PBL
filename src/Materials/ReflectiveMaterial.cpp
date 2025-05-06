#include "ReflectiveMaterial.h"

#include "Serialization/SerializationUtility.h"
#include "Shaders/ShaderManager.h"
#include "Shaders/ShaderSourceFiles.h"
#include "Engine/Textures/TextureManager.h"
#include <filesystem>

#if EDITOR
#include "imgui.h"
#endif

namespace Materials
{
    Shaders::Shader ReflectiveMaterial::DepthPass;
    Shaders::Shader ReflectiveMaterial::MainPass;
    Shaders::Shader ReflectiveMaterial::DirectionalShadowPass;
    Shaders::Shader ReflectiveMaterial::PointSpotShadowPass;

    ReflectiveMaterial::ReflectiveMaterial(const Engine::Texture EnvironmentMap) :
        Material(DepthPass, MainPass, DirectionalShadowPass, PointSpotShadowPass),
        EnvironmentMap(TextureMaterialProperty("EnvironmentMap", MainPass, EnvironmentMap))
    {
    }

    ReflectiveMaterial::ReflectiveMaterial() :
        Material(DepthPass, MainPass, DirectionalShadowPass, PointSpotShadowPass),
        EnvironmentMap("EnvironmentMap", MainPass)
    {
    }

    void ReflectiveMaterial::Initialize()
    {
        DepthPass = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/DefaultDepth/DefaultDepth.vert", nullptr,
                "./res/shaders/DefaultDepth/DefaultDepth.frag"));
        MainPass = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/Reflective/Reflective.vert", nullptr, "./res/shaders/Reflective/Reflective.frag"));
        DirectionalShadowPass = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/Common/BasicShadowPass/DirectionalLight.vert", nullptr,
                "./res/shaders/Common/BasicShadowPass/DirectionalLight.frag"));
        PointSpotShadowPass = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/Common/BasicShadowPass/PointSpotLight.vert",
                "./res/shaders/Common/BasicShadowPass/PointSpotLight.geom",
                "./res/shaders/Common/BasicShadowPass/PointSpotLight.frag"));
    }


    void ReflectiveMaterial::UseDepthPass() const
    {
        GetDepthPass().Use();
    }

    void ReflectiveMaterial::Use() const
    {
        GetMainPass().Use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, EnvironmentMap.GetValue().GetId());
    }

    void ReflectiveMaterial::UseDirectionalShadows() const
    {
        GetDirectionalShadowPass().Use();
    }

    void ReflectiveMaterial::UsePointSpotShadows() const
    {
        GetPointSpotShadowPass().Use();
    }

#if EDITOR
    void ReflectiveMaterial::DrawImGui()
    {
        static bool showEnvironmentMapPopup = false;
        static std::vector<std::string> availableTextures;
        std::string texturePath = std::filesystem::absolute("./res/textures").string();
        static bool scanned = false;

        if (!scanned)
        {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(texturePath))
            {
                if (entry.is_regular_file() && entry.path().extension() == ".dds")
                    availableTextures.emplace_back(entry.path().string());
            }
            scanned = true;
        }

        std::string environmentMapPath =
                EnvironmentMap.GetValue().GetId() != 0
                    ? Engine::TextureManager::GetTexturePath(EnvironmentMap.GetValue())
                    : "None";
        ImGui::Separator();

        ImGui::Text("Environment Map:");
        ImGui::Selectable(environmentMapPath.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick);
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH"))
            {
                const char* droppedPath = static_cast<const char*>(payload->Data);
                if (std::filesystem::path(droppedPath).extension() == ".dds")
                {
                    EnvironmentMap.SetValue(Engine::TextureManager::GetTexture(droppedPath));
                }
            }
            ImGui::EndDragDropTarget();
        }
        if (ImGui::IsItemClicked())
            showEnvironmentMapPopup = true;
        if (showEnvironmentMapPopup)
        {
            ImGui::OpenPopup("Environment Map Picker");
            showEnvironmentMapPopup = false;
        }
        if (ImGui::BeginPopup("Environment Map Picker"))
        {
            for (const auto& path : availableTextures)
            {
                std::filesystem::path fsPath(path);
                std::string displayName = std::filesystem::relative(fsPath, texturePath).string();
                if (ImGui::Selectable(displayName.c_str()))
                {
                    EnvironmentMap.SetValue(Engine::TextureManager::GetTexture(path.c_str()));
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                ImGui::TextDisabled("(%s)", path.c_str());
            }
            ImGui::EndPopup();
        }
    }
#endif

    rapidjson::Value ReflectiveMaterial::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_MATERIAL_SERIALIZATION
        SERIALIZE_PROPERTY(EnvironmentMap);
        END_MATERIAL_SERIALIZATION
    }

    void ReflectiveMaterial::Deserialize(const rapidjson::Value& Object)
    {
        START_MATERIAL_DESERIALIZATION
        DESERIALIZE_PROPERTY(EnvironmentMap);
        END_MATERIAL_DESERIALIZATION
    }
} // Models
