#include "Engine/Rendering/Plane.h"
#if EDITOR
#include "SceneViewGUI.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "Engine/EngineObjects/GizmoManager.h"
#include "Engine/EngineObjects/Scene/Scene.h"
#include "Engine/Gui/SceneHierarchyGUI.h"
#include "Engine/EngineObjects/RayCast.h"
#include "Engine/Components/Renderers/ModelRenderer.h"
#include "spdlog/spdlog.h"
#include "Engine/Components/BloodSystem/BloodSourceBase.h"

void RaycastRecursive(Engine::Transform* obj, const glm::vec3& rayOrigin, const glm::vec3& rayDir,
                      Engine::Transform*& outClosest, float& outClosestT)
{
    const Engine::ModelRenderer* renderer = obj->GetOwner()->GetComponent<Engine::ModelRenderer>();
    if (renderer && renderer->GetModel())
    {
        const glm::mat4 modelMatrix = obj->GetLocalToWorldMatrix();
        const Models::Model* model = renderer->GetModel();

        for (int i = 0; i < model->GetMeshCount(); ++i)
        {
            const Models::Mesh* mesh = model->GetMesh(i);
            const std::vector<Models::Vertex>& vertices = mesh->VerticesData;
            const std::vector<unsigned>& indices = mesh->VertexIndices;

            for (size_t j = 0; j < indices.size(); j += 3)
            {
                glm::vec3 v0 = glm::vec3(modelMatrix * glm::vec4(vertices[indices[j + 0]].Position, 1.0f));
                glm::vec3 v1 = glm::vec3(modelMatrix * glm::vec4(vertices[indices[j + 1]].Position, 1.0f));
                glm::vec3 v2 = glm::vec3(modelMatrix * glm::vec4(vertices[indices[j + 2]].Position, 1.0f));

                glm::vec3 hitPoint;
                if (Engine::RayCast::RayIntersectsTriangle(rayOrigin, rayDir, v0, v1, v2, &hitPoint))
                {
                    float t = glm::length(hitPoint - rayOrigin);
                    if (t < outClosestT)
                    {
                        outClosestT = t;
                        outClosest = obj;
                    }
                }
            }
        }
    }
    else if (const Engine::BloodSourceBase* bloodSource = obj->GetOwner()->GetComponent<Engine::BloodSourceBase>())
    {
        const glm::mat4 modelMatrix = obj->GetLocalToWorldMatrix();

        const uint32_t* const indices = Engine::Plane::GetIndices();

        for (size_t j = 0; j < Engine::Plane::GetFaceCount(); j += 3)
        {
            glm::vec3 v0 = glm::vec3(
                    modelMatrix * glm::vec4(Engine::Plane::GetVertexPosition(indices[j + 0]), 1.0f));
            glm::vec3 v1 = glm::vec3(modelMatrix * glm::vec4(Engine::Plane::GetVertexPosition(indices[j + 1]), 1.0f));
            glm::vec3 v2 = glm::vec3(modelMatrix * glm::vec4(Engine::Plane::GetVertexPosition(indices[j + 2]), 1.0f));

            glm::vec3 hitPoint;
            if (Engine::RayCast::RayIntersectsTriangle(rayOrigin, rayDir, v0, v1, v2, &hitPoint))
            {
                float t = glm::length(hitPoint - rayOrigin);
                if (t < outClosestT)
                {
                    outClosestT = t;
                    outClosest = obj;
                }
            }
        }
    }

    for (Engine::Transform* child : obj->GetChildren())
    {
        RaycastRecursive(child, rayOrigin, rayDir, outClosest, outClosestT);
    }
}

void CalculateRayFromMouse(float ndcX, float ndcY, const glm::mat4& projection, const glm::mat4& view,
                           glm::vec3& rayOrigin, glm::vec3& rayDirection)
{
    glm::vec4 rayClip(ndcX, ndcY, -1.0f, 1.0f);

    glm::vec4 rayEye = glm::inverse(projection) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

    glm::vec4 rayWorld = glm::inverse(view) * rayEye;
    rayDirection = glm::normalize(glm::vec3(rayWorld));
    rayOrigin = glm::vec3(glm::inverse(view)[3]);
}

void Engine::SceneViewGUI::Draw(CameraRenderData renderData, Scene* scene)
{
    ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoBringToFrontOnFocus);

    ImVec2 avail = ImGui::GetContentRegionAvail();

    // Compute 16:9 aspect ratio size
    float aspectWidth = avail.x;
    float aspectHeight = aspectWidth * 9.0f / 16.0f;

    if (aspectHeight > avail.y)
    {
        aspectHeight = avail.y;
        aspectWidth = aspectHeight * 16.0f / 9.0f;
    }

    ImVec2 imageSize = ImVec2(aspectWidth, aspectHeight);

    // Get top-left corner position for the centered image
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    ImVec2 imagePos =
            ImVec2(cursorPos.x + (avail.x - imageSize.x) * 0.5f, cursorPos.y + (avail.y - imageSize.y) * 0.5f);

    // Set ImGuizmo rect (must be in screen space)
    ImGuizmo::BeginFrame();
    ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
    ImGuizmo::SetRect(imagePos.x, imagePos.y, imageSize.x, imageSize.y);
    GizmoManager::GetInstance()->SetRect(imagePos.x, imagePos.y, imageSize.x, imageSize.y);

    // Draw the image (framebuffer)
    if (m_FramebufferTexture != 0)
    {
        ImGui::SetCursorScreenPos(imagePos);
        ImGui::Image((ImTextureID) (uintptr_t) m_FramebufferTexture, imageSize, ImVec2(0, 1), ImVec2(1, 0));
    }
    else
    {
        ImGui::Text("No framebuffer texture set.");
    }

    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGuizmo::IsOver())
    {
        ImVec2 mousePos = ImGui::GetIO().MousePos;

        float mouseX = mousePos.x - imagePos.x;
        float mouseY = mousePos.y - imagePos.y;

        if (mouseX >= 0 && mouseX <= imageSize.x &&
            mouseY >= 0 && mouseY <= imageSize.y)
        {
            float normalizedX = (mouseX / imageSize.x) * 2.0f - 1.0f;
            float normalizedY = 1.0f - (mouseY / imageSize.y) * 2.0f;

            glm::vec3 rayOrigin, rayDirection;
            CalculateRayFromMouse(normalizedX, normalizedY, renderData.ProjectionMatrix, renderData.ViewMatrix,
                                  rayOrigin, rayDirection);

            Transform* selectedObject = nullptr;
            float closestT = std::numeric_limits<float>::max();

            RaycastRecursive(scene->GetRoot()->GetTransform(), rayOrigin, rayDirection, selectedObject, closestT);

            if (selectedObject)
            {
                GizmoManager::GetInstance()->SetManaged(selectedObject);
                SceneHierarchyGUI::GetInstance()->SetSelectedEntity(selectedObject);
            }
            else
            {
                GizmoManager::GetInstance()->SetManaged(nullptr);
                SceneHierarchyGUI::GetInstance()->SetSelectedEntity(nullptr);
            }
        }
    }

    ImGui::End();
}
#endif
