#include "WaterMaterial.h"
#include "GLFW/glfw3.h"
#include "Serialization/SerializationUtility.h"
#include "Shaders/ShaderManager.h"
#include "Shaders/ShaderSourceFiles.h"
#include "Engine/Textures/TextureManager.h"
#include "imgui.h"
#include <filesystem>

#include "Engine/EngineObjects/LightManager.h"

namespace Materials
{
    Shaders::Shader WaterMaterial::DepthPass;
    Shaders::Shader WaterMaterial::MainPass;
    Shaders::Shader WaterMaterial::DirectionalShadowPass;
    Shaders::Shader WaterMaterial::PointSpotShadowPass;

    WaterMaterial::WaterMaterial(const Engine::Texture NormalMap0, const Engine::Texture NormalMap1,
                                 const glm::vec3& Color, const glm::vec2& Tiling0, const glm::vec2& Tiling1,
                                 const glm::vec2& Velocity0, const glm::vec2& Velocity1, const float Roughness,
                                 const float Metallic) :
        Material(DepthPass, MainPass, DirectionalShadowPass, PointSpotShadowPass),
        NormalMap0(TextureMaterialProperty("NormalMap0", MainPass, NormalMap0)),
        NormalMap1(TextureMaterialProperty("NormalMap1", MainPass, NormalMap1)),
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
        Material(DepthPass, MainPass, DirectionalShadowPass, PointSpotShadowPass),
        NormalMap0(TextureMaterialProperty("NormalMap0", MainPass)),
        NormalMap1(TextureMaterialProperty("NormalMap1", MainPass)), Color(Vector3MaterialProperty("Color", MainPass)),
        Tiling0(Vector2MaterialProperty("Tiling0", MainPass)), Tiling1(Vector2MaterialProperty("Tiling1", MainPass)),
        Velocity0(Vector2MaterialProperty("Velocity0", MainPass)), Velocity1("Velocity1", MainPass),
        Roughness(FloatMaterialProperty("Roughness", MainPass)), Metallic(FloatMaterialProperty("Metallic", MainPass)),
        TimeLocation(MainPass.GetUniformLocation("Time"))
    {
    }

    void WaterMaterial::Initialize()
    {
        DepthPass = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/DefaultDepth/DefaultDepth.vert", nullptr,
                "./res/shaders/DefaultDepth/DefaultDepth.frag"));
        MainPass = Shaders::ShaderManager::GetShader(
                Shaders::ShaderSourceFiles("./res/shaders/Water/Water.vert", nullptr,
                                           "./res/shaders/Water/Water.frag"));
        DirectionalShadowPass = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/Common/BasicShadowPass/DirectionalLight.vert", nullptr,
                "./res/shaders/Common/BasicShadowPass/DirectionalLight.frag"));
        PointSpotShadowPass = Shaders::ShaderManager::GetShader(Shaders::ShaderSourceFiles(
                "./res/shaders/Common/BasicShadowPass/PointSpotLight.vert",
                "./res/shaders/Common/BasicShadowPass/PointSpotLight.geom",
                "./res/shaders/Common/BasicShadowPass/PointSpotLight.frag"));
    }

    void WaterMaterial::UseDepthPass() const
    {
        GetDepthPass().Use();
    }

    void WaterMaterial::Use() const
    {
        GetMainPass().Use();
        Engine::LightManager::GetInstance()->SetupLightsForRendering(MainPass);

        Color.Bind();
        Roughness.Bind();
        Metallic.Bind();
        Tiling0.Bind();
        Tiling1.Bind();
        Velocity0.Bind();
        Velocity1.Bind();

        NormalMap0.Bind();
        NormalMap1.Bind();

        Shaders::Shader::SetUniform(TimeLocation, static_cast<float>(glfwGetTime()));
    }

    void WaterMaterial::UseDirectionalShadows() const
    {
        GetDirectionalShadowPass().Use();
    }

    void WaterMaterial::UsePointSpotShadows() const
    {
        GetPointSpotShadowPass().Use();
    }
#if EDITOR
    void WaterMaterial::DrawImGui()
    {
        {
            static bool showNormal0Popup = false;
            static bool showNormal1Popup = false;
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

            std::string normalMap0Path =
                    NormalMap0.GetValue().GetId() != 0
                        ? Engine::TextureManager::GetTexturePath(NormalMap0.GetValue())
                        : "None";
            std::string normalMap1Path =
                    NormalMap1.GetValue().GetId() != 0
                        ? Engine::TextureManager::GetTexturePath(NormalMap1.GetValue())
                        : "None";


            ImGui::Text("Normal Map 0:");
            ImGui::Selectable(normalMap0Path.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick);
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH"))
                {
                    const char* droppedPath = static_cast<const char*>(payload->Data);
                    if (std::filesystem::path(droppedPath).extension() == ".dds")
                    {
                        NormalMap0.SetValue(Engine::TextureManager::GetTexture(droppedPath));
                    }
                }
                ImGui::EndDragDropTarget();
            }
            if (ImGui::IsItemClicked())
                showNormal0Popup = true;
            if (showNormal0Popup)
            {
                ImGui::OpenPopup("Normal Map 0 Picker");
                showNormal0Popup = false;
            }
            if (ImGui::BeginPopup("Normal Map 0 Picker"))
            {
                for (const auto& path : availableTextures)
                {
                    std::filesystem::path fsPath(path);
                    std::string displayName = std::filesystem::relative(fsPath, texturePath).string();
                    if (ImGui::Selectable(displayName.c_str()))
                    {
                        NormalMap0.SetValue(Engine::TextureManager::GetTexture(path.c_str()));
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::SameLine();
                    ImGui::TextDisabled("(%s)", path.c_str());
                }
                ImGui::EndPopup();
            }
            ImGui::Text("Normal Map 1:");
            ImGui::Selectable(normalMap1Path.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick);
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH"))
                {
                    const char* droppedPath = static_cast<const char*>(payload->Data);
                    if (std::filesystem::path(droppedPath).extension() == ".dds")
                    {
                        NormalMap1.SetValue(Engine::TextureManager::GetTexture(droppedPath));
                    }
                }
                ImGui::EndDragDropTarget();
            }
            if (ImGui::IsItemClicked())
                showNormal1Popup = true;
            if (showNormal1Popup)
            {
                ImGui::OpenPopup("Normal Map 1 Picker");
                showNormal1Popup = false;
            }
            if (ImGui::BeginPopup("Normal Map 1 Picker"))
            {
                for (const auto& path : availableTextures)
                {
                    std::filesystem::path fsPath(path);
                    std::string displayName = std::filesystem::relative(fsPath, texturePath).string();
                    if (ImGui::Selectable(displayName.c_str()))
                    {
                        NormalMap1.SetValue(Engine::TextureManager::GetTexture(path.c_str()));
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::SameLine();
                    ImGui::TextDisabled("(%s)", path.c_str());
                }
                ImGui::EndPopup();
            }

            ImGui::Separator();

            glm::vec3 color = Color.GetValue();
            if (ImGui::ColorEdit3("Color", &color.x))
            {
                Color.SetValue(color);
            }

            glm::vec2 tiling0 = Tiling0.GetValue();
            if (ImGui::DragFloat2("Tiling 0", &tiling0.x, 0.01f))
            {
                Tiling0.SetValue(tiling0);
            }

            glm::vec2 tiling1 = Tiling1.GetValue();
            if (ImGui::DragFloat2("Tiling 1", &tiling1.x, 0.01f))
            {
                Tiling1.SetValue(tiling1);
            }

            glm::vec2 velocity0 = Velocity0.GetValue();
            if (ImGui::DragFloat2("Velocity 0", &velocity0.x, 0.01f))
            {
                Velocity0.SetValue(velocity0);
            }

            glm::vec2 velocity1 = Velocity1.GetValue();
            if (ImGui::DragFloat2("Velocity 1", &velocity1.x, 0.01f))
            {
                Velocity1.SetValue(velocity1);
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
        }
    }
#endif
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
