#include "SkyboxRenderer.h"

#include "Serialization/SerializationUtility.h"
#include "Engine/EngineObjects/Entity.h"

#if EDITOR
#include <filesystem>
#include "imgui.h"
namespace fs = std::filesystem;
#endif

namespace Engine
{
    void SkyboxRenderer::RenderDepth(const CameraRenderData& RenderData)
    {
    }

    void SkyboxRenderer::Render(const CameraRenderData& RenderData)
    {
        const Shaders::Shader shader = GetMaterial()->GetMainPass();
        shader.SetUniform("ViewMatrix", RenderData.ViewMatrix);
        shader.SetUniform("ProjectionMatrix", RenderData.ProjectionMatrix);
        Cube.Draw();
    }

    void SkyboxRenderer::RenderDirectionalShadows(const CameraRenderData& RenderData)
    {
    }

    void SkyboxRenderer::RenderPointSpotShadows(const glm::vec3& LightPosition, float LightRange,
                                                const glm::mat4* SpaceTransformMatrices)
    {
    }

#if EDITOR
    void SkyboxRenderer::DrawImGui()
    {
        if (ImGui::CollapsingHeader("Skybox Renderer", ImGuiTreeNodeFlags_DefaultOpen))
        {
            static bool showMaterialPopup = false;
            static bool showModelPopup = false;


            static std::vector<std::string> availableMaterials;
            static std::vector<std::string> availableModels;


            static bool scanned = false;
            std::string materialPath = fs::absolute("./res/materials/SampleScene").string();
            std::string modelPath = fs::absolute("./res/Models").string();
            if (!scanned)
            {
                for (const auto& entry : fs::directory_iterator(materialPath))
                {
                    if (entry.is_regular_file() && entry.path().extension() == ".mat")
                        availableMaterials.emplace_back(entry.path().string());
                }
                for (const auto& entry : fs::directory_iterator(modelPath))
                {
                    if (entry.is_regular_file() && entry.path().extension() == ".fbx")
                        availableModels.emplace_back(entry.path().string());
                }

                scanned = true;
            }


            if (Material)
            {
                ImGui::Text("Material Properties:");
                ImGui::Separator();
                Material->DrawImGui();
                ImGui::Separator();
            }
            ImGui::Text("Material file:");
            materialPath = Material ? Materials::MaterialManager::GetMaterialPath(Material) : "None";


            ImGui::Selectable(materialPath.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick);

            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH"))
                {
                    const char* droppedPath = static_cast<const char*>(payload->Data);
                    if (fs::path(droppedPath).extension() == ".mat")
                    {
                        SetMaterial(Materials::MaterialManager::GetMaterial(droppedPath));
                    }
                }
                ImGui::EndDragDropTarget();
            }

            if (ImGui::IsItemClicked())
                showMaterialPopup = true;

            if (showMaterialPopup)
            {
                ImGui::OpenPopup("Material Picker");
                showMaterialPopup = false;
            }

            if (ImGui::BeginPopup("Material Picker"))
            {
                for (const auto& path : availableMaterials)
                {
                    std::filesystem::path fsPath(path);
                    std::string displayName = fsPath.filename().string();

                    if (ImGui::Selectable(displayName.c_str()))
                    {
                        SetMaterial(Materials::MaterialManager::GetMaterial(path));
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::SameLine();
                    ImGui::TextDisabled("(%s)", path.c_str());
                }
                ImGui::EndPopup();
            }

            static char editableMaterialName[256] = {};
            static std::string lastMaterialPath;

            std::string currentMaterialPath = materialPath;
            if (currentMaterialPath != lastMaterialPath)
            {
                strncpy_s(editableMaterialName, fs::path(currentMaterialPath).filename().string().c_str(),
                          sizeof(editableMaterialName) - 1);
                lastMaterialPath = currentMaterialPath;
            }

            ImGui::InputText("Material File Name", editableMaterialName, sizeof(editableMaterialName));

            if (ImGui::Button("Save"))
            {
                if (!std::string(editableMaterialName).empty())
                {
                    fs::path newMaterialPath = fs::path("./res/materials/SampleScene");
                    Materials::MaterialManager::SaveMaterial(newMaterialPath.string() + "/" + editableMaterialName,
                                                             Material);
                }
            }
            ImGui::Separator();
        }
    }
# endif
    rapidjson::Value SkyboxRenderer::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_COMPONENT_SERIALIZATION
        SERIALIZE_FIELD(Material)
        END_COMPONENT_SERIALIZATION
    }

    void SkyboxRenderer::DeserializeValuePass(const rapidjson::Value& Object,
                                              Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        DESERIALIZE_VALUE(Material)
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void SkyboxRenderer::DeserializeReferencesPass(const rapidjson::Value& Object,
                                                   Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
        END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    }
} // Engine
