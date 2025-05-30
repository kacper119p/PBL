#include "RefractiveMaterial.h"

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
    Shaders::Shader RefractiveMaterial::DepthPass;
    Shaders::Shader RefractiveMaterial::MainPass;
    Shaders::Shader RefractiveMaterial::DirectionalShadowPass;
    Shaders::Shader RefractiveMaterial::PointSpotShadowPass;

    RefractiveMaterial::RefractiveMaterial(const Engine::Texture EnvironmentMap, const float IndexOfRefraction) :
        Material(DepthPass, MainPass, DirectionalShadowPass, PointSpotShadowPass),
        EnvironmentMap(TextureMaterialProperty("EnvironmentMap", MainPass, EnvironmentMap)),
        IndexOfRefraction(FloatMaterialProperty("IOR", MainPass, IndexOfRefraction))
    {
    }

    RefractiveMaterial::RefractiveMaterial() :
        Material(DepthPass, MainPass, DirectionalShadowPass, PointSpotShadowPass),
        EnvironmentMap(TextureMaterialProperty("EnvironmentMap", MainPass)),
        IndexOfRefraction(FloatMaterialProperty("IOR", MainPass))
    {
    }

    void RefractiveMaterial::Initialize()
    {
        DepthPass = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/DefaultDepth/DefaultDepth.vert", nullptr,
                "./res/shaders/DefaultDepth/DefaultDepth.frag"));
        MainPass = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/Refractive/Refractive.vert", nullptr, "./res/shaders/Refractive/Refractive.frag"));
        DirectionalShadowPass = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/Common/BasicShadowPass/DirectionalLight.vert", nullptr,
                "./res/shaders/Common/BasicShadowPass/DirectionalLight.frag"));
        PointSpotShadowPass = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/Common/BasicShadowPass/PointSpotLight.vert",
                "./res/shaders/Common/BasicShadowPass/PointSpotLight.geom",
                "./res/shaders/Common/BasicShadowPass/PointSpotLight.frag"));
    }


    void RefractiveMaterial::UseDepthPass() const
    {
        GetDepthPass().Use();
    }

    void RefractiveMaterial::Use() const
    {
        GetMainPass().Use();

        IndexOfRefraction.Bind();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, EnvironmentMap.GetValue().GetId());
    }

    void RefractiveMaterial::UseDirectionalShadows() const
    {
        GetDirectionalShadowPass().Use();
    }

    void RefractiveMaterial::UsePointSpotShadows() const
    {
        GetPointSpotShadowPass().Use();
    }
#if EDITOR
    void RefractiveMaterial::DrawImGui()
    {
        float ior = IndexOfRefraction.GetValue();
        if (ImGui::SliderFloat("Index of Refraction", &ior, 1.0f, 2.5f))
        {
            IndexOfRefraction.SetValue(ior);
        }

        static bool showEnvironmentMapPopup = false;
        static std::vector<std::string> availableTextures;
        std::string texturePath = std::filesystem::absolute("./res/textures").string();
        static bool scanned = false;

        if (!scanned)
        {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(texturePath))
            {
                if (entry.is_regular_file() && entry.path().extension() == ".dds")
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
    rapidjson::Value RefractiveMaterial::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_MATERIAL_SERIALIZATION
        SERIALIZE_PROPERTY(EnvironmentMap);
        SERIALIZE_PROPERTY(IndexOfRefraction);
        END_MATERIAL_SERIALIZATION
    }

    void RefractiveMaterial::Deserialize(const rapidjson::Value& Object)
    {
        START_MATERIAL_DESERIALIZATION
        DESERIALIZE_PROPERTY(EnvironmentMap);
        DESERIALIZE_PROPERTY(IndexOfRefraction);
        END_MATERIAL_DESERIALIZATION
    }
} // Models
