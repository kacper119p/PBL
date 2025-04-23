#if EDITOR
#include "SceneHierarchyGUI.h"

#include "TransformGui.h"
#include "Engine/EngineObjects/GizmoManager.h"
#include "Engine/EngineObjects/Scene/Scene.h"
#include "Engine/Components/Renderers/ModelRenderer.h"

#include "imgui.h"
Engine::SceneHierarchyGUI::SceneHierarchyGUI(Transform* Root) : Root(Root), SelectedEntity(Root){}

void Engine::SceneHierarchyGUI::DrawHierarchy(Transform* entity, Scene* scene)
{
    if (!entity)
        return;

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
    if (entity == SelectedEntity)
        flags |= ImGuiTreeNodeFlags_Selected;

    const bool hasChildren = !entity->GetChildren().empty();
    if (!hasChildren)
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

    if (entity == Root)
        flags |= ImGuiTreeNodeFlags_DefaultOpen;

    // Push a unique ID to keep context safe even with NoTreePushOnOpen
    ImGui::PushID((void*) entity);

    // Tree node
    bool open = ImGui::TreeNodeEx((void*) entity, flags, "%s", entity->GetOwner()->GetName().c_str());

    // Handle left-click
    if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
        SelectedEntity = entity;

    // Right-click context menu (now safe because of PushID)
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Add Entity"))
        {
            AddEntityToScene(scene, entity->GetOwner());
        }
        if (ImGui::MenuItem("Add Model"))
        {
            AddModelToScene(scene, entity->GetOwner());
        }
        if (ImGui::MenuItem("Add Animated Model"))
        {
            AddAnimatedModelToScene(scene, entity->GetOwner());
        }

        ImGui::EndPopup();
    }

    if (open && hasChildren)
    {
        for (Transform* child : entity->GetChildren())
            DrawHierarchy(child, scene);

        ImGui::TreePop();
    }

    ImGui::PopID(); // Always pair with PushID
}


void Engine::SceneHierarchyGUI::Draw(Scene* scene)
{
    ImGui::Begin("Hierarchy");

    if (Root)
    {
        // Instead of drawing the root, draw its children directly
        for (Transform* child : Root->GetChildren())
        {
            DrawHierarchy(child, scene);
        }

        if (ImGui::BeginPopupContextWindow("HierarchyContext",
                                           ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
        {
            if (ImGui::MenuItem("Add Entity"))
            {
                AddEntityToScene(scene, Root->GetOwner());
            }
            if (ImGui::MenuItem("Add Model"))
            {
                AddModelToScene(scene, Root->GetOwner());
            }
            if (ImGui::MenuItem("Add Animated Model"))
            {
                AddAnimatedModelToScene(scene, Root->GetOwner());
            }

            ImGui::EndPopup();
        }


        GizmoManager::GetInstance()->SetManaged(SelectedEntity);
    }

    if (SelectedEntity && ImGui::IsWindowFocused() && ImGui::IsKeyPressed(ImGuiKey_Delete))
    {
        if (SelectedEntity != Root)
        {
            scene->DeleteEntity(SelectedEntity->GetOwner());
            SelectedEntity = Root; // Reset to root after deletion
        }
    }

    ImGui::End();
}



void Engine::SceneHierarchyGUI::AddEntityToScene(Scene* scene, Entity* parent)
{
    Entity* entity = scene->SpawnEntity(parent);
}

void Engine::SceneHierarchyGUI::AddModelToScene(Scene* scene, Entity* parent)
{
    Entity* entity = scene->SpawnEntity(parent);
    entity->AddComponent<ModelRenderer>();
}

void Engine::SceneHierarchyGUI::AddAnimatedModelToScene(Scene* scene, Entity* parent)
{
    Entity* entity = scene->SpawnEntity(parent);
}
#endif
