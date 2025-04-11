#include "SceneHierarchyGUI.h"

#include "TransformGui.h"
#include "Engine/EngineObjects/GizmoManager.h"
#include "imgui.h"
Engine::SceneHierarchyGUI::SceneHierarchyGUI(Transform* Root) : Root(Root), SelectedEntity(Root){}

void Engine::SceneHierarchyGUI::DrawHierarchy(Transform* entity) {
    if (!entity)
        return;

    // Apply selected flag if this entity is selected
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
    if (entity == SelectedEntity)
        flags |= ImGuiTreeNodeFlags_Selected;

    if (entity->GetChildren().empty())
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

    if (entity == Root)
        flags |= ImGuiTreeNodeFlags_DefaultOpen;

    // Tree node with pointer ID (avoids duplicates)
    bool open = ImGui::TreeNodeEx((void*) (intptr_t) entity, flags, "%s", entity->GetOwner());

    // Handle click selection
    if (ImGui::IsItemClicked())
        SelectedEntity = entity;

    // Recurse if node is open and has children
    if (open && !entity->GetChildren().empty())
    {
        for (Transform* child : entity->GetChildren())
            DrawHierarchy(child);

        ImGui::TreePop();
    }
}

void Engine::SceneHierarchyGUI::Draw()
{
    ImGui::Begin("Hierarchy");
    ImGui::Text("Scene Hierarchy");
    if (Root)
    {
        // Draw root node (scene)
        DrawHierarchy(Root);
        GizmoManager::GetInstance()->SetManaged(SelectedEntity);
    }
    ImGui::End();
}

