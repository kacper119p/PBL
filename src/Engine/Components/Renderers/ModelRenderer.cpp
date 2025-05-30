#include "ModelRenderer.h"
#include "Engine/EngineObjects/LightManager.h"
#include "Engine/EngineObjects/CameraRenderData.h"
#include "Engine/EngineObjects/RenderingManager.h"
#include "Serialization/SerializationUtility.h"
#include "Materials/MaterialManager.h"
#include "Materials/Material.h"
#include "Models/ModelManager.h"

#if EDITOR
#include <filesystem>
#include "imgui.h"
namespace fs = std::filesystem;
#endif


namespace Engine
{
    void ModelRenderer::RenderDepth(const CameraRenderData& RenderData)
    {
        if (Model == nullptr)
        {
            Culled = true;
            return;
        }

        Culled = true;

        const auto& frustum = RenderingManager::GetInstance()->GetFrustum();
        const glm::mat4 objectToWorldMatrix = GetOwner()->GetTransform()->GetLocalToWorldMatrix();
        for (int i = 0; i < Model->GetMeshCount(); ++i)
        {
            if (frustum.IsBoxVisible(Model->GetMesh(i)->GetAabBox(), objectToWorldMatrix))
            {
                Culled &= false;
                SetupMatrices(RenderData, Material->GetDepthPass());
                Model->GetMesh(i)->Draw();
            }
        }
    }

    void ModelRenderer::Render(const CameraRenderData& RenderData)
    {
        if (Culled)
        {
            return;
        }

        SetupMatrices(RenderData, Material->GetMainPass());
        Draw();
    }

    void ModelRenderer::RenderDirectionalShadows(const CameraRenderData& RenderData)
    {
        if (Model == nullptr)
        {
            return;
        }
        SetupMatrices(RenderData, Material->GetDirectionalShadowPass());
        Draw();
    }

    void
    ModelRenderer::RenderPointSpotShadows(const glm::vec3& LightPosition, float LightRange,
                                          const glm::mat4* const SpaceTransformMatrices)
    {
        if (Model == nullptr)
        {
            return;
        }
        Material->GetPointSpotShadowPass().SetUniform("ShadowMatrices[0]", SpaceTransformMatrices[0]);
        Material->GetPointSpotShadowPass().SetUniform("ShadowMatrices[1]", SpaceTransformMatrices[1]);
        Material->GetPointSpotShadowPass().SetUniform("ShadowMatrices[2]", SpaceTransformMatrices[2]);
        Material->GetPointSpotShadowPass().SetUniform("ShadowMatrices[3]", SpaceTransformMatrices[3]);
        Material->GetPointSpotShadowPass().SetUniform("ShadowMatrices[4]", SpaceTransformMatrices[4]);
        Material->GetPointSpotShadowPass().SetUniform("ShadowMatrices[5]", SpaceTransformMatrices[5]);

        Material->GetPointSpotShadowPass().SetUniform("LightPosition", LightPosition);
        Material->GetPointSpotShadowPass().SetUniform("Range", LightRange);

        Material->GetPointSpotShadowPass().SetUniform("ObjectToWorldMatrix",
                                                      GetOwner()->GetTransform()->GetLocalToWorldMatrix());

        Draw();
    }

    void ModelRenderer::SetupMatrices(const CameraRenderData& RenderData, const Shaders::Shader& Shader) const
    {
        Shader.SetUniform("CameraPosition", RenderData.CameraPosition);
        Shader.SetUniform("ViewMatrix", RenderData.ViewMatrix);
        Shader.SetUniform("ProjectionMatrix", RenderData.ProjectionMatrix);
        Shader.SetUniform("ObjectToWorldMatrix",
                          GetOwner()->GetTransform()->GetLocalToWorldMatrix());
    }

    void ModelRenderer::Draw() const
    {
        for (int i = 0; i < Model->GetMeshCount(); ++i)
        {
            Model->GetMesh(i)->Draw();
        }
    }

#if EDITOR
    void ModelRenderer::DrawImGui()
    {
        if (ImGui::CollapsingHeader("Model Renderer", ImGuiTreeNodeFlags_DefaultOpen))
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
                for (const auto& entry : fs::recursive_directory_iterator(materialPath))
                {
                    if (entry.is_regular_file() && entry.path().extension() == ".mat")
                    {
                        std::string relPath = entry.path().lexically_normal().string();
                        std::replace(relPath.begin(), relPath.end(), '\\', '/');
                        size_t pos = relPath.find("/res/");
                        if (pos != std::string::npos)
                            relPath = "." + relPath.substr(pos);
                        availableMaterials.emplace_back(relPath);
                    }
                }
                for (const auto& entry : fs::recursive_directory_iterator(modelPath))
                {
                    if (entry.is_regular_file() && entry.path().extension() == ".fbx")
                    {
                        std::string relPath = entry.path().lexically_normal().string();
                        std::replace(relPath.begin(), relPath.end(), '\\', '/');
                        size_t pos = relPath.find("/res/");
                        if (pos != std::string::npos)
                            relPath = "." + relPath.substr(pos);
                        availableModels.emplace_back(relPath);
                    }
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
                        fs::path relPath = fs::relative(droppedPath, fs::current_path());
                        SetMaterial(Materials::MaterialManager::GetMaterial(relPath.string()));
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

                    static char currentMaterialName[256] = {};
                    strncpy_s(currentMaterialName, fsPath.filename().string().c_str(), sizeof(currentMaterialName) - 1);

                    if (ImGui::Selectable(displayName.c_str()))
                    {
                        fs::path relPath = fs::path("./res/materials/SampleScene/" + std::string(currentMaterialName));
                        SetMaterial(Materials::MaterialManager::GetMaterial(relPath.string()));
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
                    //Material = Materials::MaterialManager::GetMaterial(newMaterialPath.string() + "/" + editableMaterialName);
                }
            }


            ImGui::Separator();

            ImGui::Text("Model:");
            modelPath = Model ? Models::ModelManager::GetModelPath(Model) : "None";
            ImGui::Selectable(modelPath.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick);

            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH"))
                {
                    const char* droppedPath = static_cast<const char*>(payload->Data);
                    if (fs::path(droppedPath).extension() == ".fbx")
                    {
                        Model = Models::ModelManager::GetModel(droppedPath);
                    }
                }
                ImGui::EndDragDropTarget();
            }

            if (ImGui::IsItemClicked())
                showModelPopup = true;

            if (showModelPopup)
            {
                ImGui::OpenPopup("Model Picker");
                showModelPopup = false;
            }

            if (ImGui::BeginPopup("Model Picker"))
            {
                for (const auto& path : availableModels)
                {
                    std::filesystem::path fsPath(path);
                    std::string displayName = fsPath.filename().string();

                    static char currentModelName[256] = {};
                    strncpy_s(currentModelName, fsPath.filename().string().c_str(), sizeof(currentModelName) - 1);

                    if (ImGui::Selectable(displayName.c_str()))
                    {
                        fs::path relPath = fs::path("./res/models/" + std::string(currentModelName));
                        Model = Models::ModelManager::GetModel(relPath.string().c_str());
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::SameLine();
                    ImGui::TextDisabled("(%s)", path.c_str());
                }
                ImGui::EndPopup();
            }

        }
    }
#endif

    rapidjson::Value ModelRenderer::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_COMPONENT_SERIALIZATION
        SERIALIZE_FIELD(Material)
        SERIALIZE_FIELD(Model)
        END_COMPONENT_SERIALIZATION
    }

    void ModelRenderer::DeserializeValuePass(const rapidjson::Value& Object,
                                             Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        DESERIALIZE_VALUE(Material)
        DESERIALIZE_VALUE(Model)
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void ModelRenderer::DeserializeReferencesPass(const rapidjson::Value& Object,
                                                  Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
        END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    }
} // Engine
