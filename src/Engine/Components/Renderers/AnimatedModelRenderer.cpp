#include "AnimatedModelRenderer.h"
#include "Engine/EngineObjects/CameraRenderData.h"
#include "Engine/EngineObjects/LightManager.h"
#include "ModelRenderer.h"
#include "Models/ModelAnimated.h"
#include "Models/Animator.h"
#if EDITOR
#include "Materials/MaterialManager.h"
#include "Materials/Material.h"
#include "Models/ModelManager.h"
#include "imgui.h"
#include <filesystem>
namespace fs = std::filesystem;
#endif
namespace Engine
{

    AnimatedModelRenderer::~AnimatedModelRenderer()
    {
        {
            // Remove this instance from the UpdateManager
            UpdateManager::GetInstance()->UnregisterComponent(this);
        }

    }

    void AnimatedModelRenderer::Start()
    {
        Renderer::Start();
        UpdateManager::GetInstance()->RegisterComponent(this);
    }

    void AnimatedModelRenderer::RenderDepth(const CameraRenderData& RenderData)
    {
        if (Model == nullptr || Animation == nullptr)
        {
            return;
        }
        SetupMatrices(RenderData, Material->GetDepthPass());
        Draw();
    }

    void AnimatedModelRenderer::Render(const CameraRenderData& RenderData)
    {
        if (Model == nullptr || Animation == nullptr)
        {
            return;
        }
        SetupMatrices(RenderData, Material->GetMainPass());
        Draw();
    }

    void AnimatedModelRenderer::RenderDirectionalShadows(const CameraRenderData& RenderData)
    {
        if (Model == nullptr || Animation == nullptr)
        {
            return;
        }
        SetupMatrices(RenderData, Material->GetDirectionalShadowPass());
        Draw();
    }

    void AnimatedModelRenderer::RenderPointSpotShadows(const glm::vec3& LightPosition, float LightRange,
                                                       const glm::mat4* const SpaceTransformMatrices)
    {
        if (Model == nullptr || Animation == nullptr)
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

    void AnimatedModelRenderer::SetupMatrices(const CameraRenderData& RenderData, const Shaders::Shader& Shader) const
    {
        Shader.SetUniform("CameraPosition", RenderData.CameraPosition);
        Shader.SetUniform("ViewMatrix", RenderData.ViewMatrix);
        Shader.SetUniform("ProjectionMatrix", RenderData.ProjectionMatrix);
        Shader.SetUniform("ObjectToWorldMatrix", GetOwner()->GetTransform()->GetLocalToWorldMatrix());
        std::vector<glm::mat4> transforms = Animator.GetFinalBoneMatrices();
        for (int i = 0; i < transforms.size(); ++i)
        {
            glm::mat4 transform = transforms[i];
            Shader.SetUniform(("finalBonesMatrices[" + std::to_string(i) + "]").c_str(), transform);
        }
    }

    void AnimatedModelRenderer::Draw() const
    {
        for (int i = 0; i < Model->GetMeshCount(); ++i)
        {
            Model->GetMesh(i)->Draw();
        }
    }
#if EDITOR
    void AnimatedModelRenderer::DrawImGui()
    {
        if (ImGui::CollapsingHeader("Animated Model Renderer", ImGuiTreeNodeFlags_DefaultOpen))
        {
            static bool showMaterialPopup = false;
            static bool showModelPopup = false;
            static bool showAnimationPopup = false;

            static std::vector<std::string> availableMaterials;
            static std::vector<std::string> availableModels;

            static bool scanned = false;

            std::string materialPath = fs::relative("./res/materials/SampleScene").string();
            std::string modelPath = fs::relative("./res/Models").string();

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
                        std::filesystem::path relPath = fs::relative(path);
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
                    // Material = Materials::MaterialManager::GetMaterial(newMaterialPath.string() + "/" +
                    // editableMaterialName);
                }
            }


            ImGui::Separator();

            ImGui::Text("Model:");
            modelPath = Model ? Models::ModelManager::GetAnimatedModelPath(Model) : "None";
            ImGui::Selectable(modelPath.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick);

            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH"))
                {
                    const char* droppedPath = static_cast<const char*>(payload->Data);
                    if (fs::path(droppedPath).extension() == ".fbx")
                    {
                        Model = Models::ModelManager::GetAnimatedModel(droppedPath);
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

                    if (ImGui::Selectable(displayName.c_str()))
                    {
                        Model = Models::ModelManager::GetAnimatedModel(path.c_str());
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::SameLine();
                    ImGui::TextDisabled("(%s)", path.c_str());
                }
                ImGui::EndPopup();
            }

            ImGui::Separator();

            ImGui::Text("Animation:");
            modelPath = Model ? Models::ModelManager::GetAnimationPath(Animation) : "None";
            ImGui::Selectable(modelPath.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick);

            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH"))
                {
                    const char* droppedPath = static_cast<const char*>(payload->Data);
                    if (fs::path(droppedPath).extension() == ".fbx")
                    {
                        Model = Models::ModelManager::GetAnimatedModel(droppedPath);
                    }
                }
                ImGui::EndDragDropTarget();
            }

            if (ImGui::IsItemClicked())
                showAnimationPopup = true;

            if (showAnimationPopup)
            {
                ImGui::OpenPopup("Animation Picker");
                showAnimationPopup = false;
            }

            if (ImGui::BeginPopup("Animation Picker"))
            {
                for (const auto& path : availableModels)
                {
                    std::filesystem::path fsPath(path);
                    std::string displayName = fsPath.filename().string();

                    if (ImGui::Selectable(displayName.c_str()))
                    {
                        Model = Models::ModelManager::GetAnimatedModel(path.c_str());
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
    rapidjson::Value AnimatedModelRenderer::Serialize(rapidjson::Document::AllocatorType& Allocator) const
    {
        START_COMPONENT_SERIALIZATION
        SERIALIZE_FIELD(Material)
        SERIALIZE_FIELD(Model)
        SERIALIZE_FIELD(Animation)
        END_COMPONENT_SERIALIZATION
    }

    void AnimatedModelRenderer::DeserializeValuePass(const rapidjson::Value& Object,
                                                     Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_VALUE_PASS
        DESERIALIZE_VALUE(Material)
        DESERIALIZE_VALUE(Model)
        DESERIALIZE_VALUE(Animation)
        Animator = Models::Animator(Animation);
        END_COMPONENT_DESERIALIZATION_VALUE_PASS
    }

    void AnimatedModelRenderer::DeserializeReferencesPass(const rapidjson::Value& Object,
                                                          Serialization::ReferenceTable& ReferenceMap)
    {
        START_COMPONENT_DESERIALIZATION_REFERENCES_PASS
        END_COMPONENT_DESERIALIZATION_REFERENCES_PASS
    }

    void AnimatedModelRenderer::Update(float DeltaTime) { Animator.UpdateAnimation(DeltaTime); }
}
